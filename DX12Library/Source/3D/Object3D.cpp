#include "Object3D.h"
#include "DX12Util.h"
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "PostEffect.h"
#include "DrawManager.h"
#include "Archive.h"
#include "Encorder.h"
#include <typeinfo>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DX12Library;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Object3D::device = nullptr;
Camera* Object3D::camera = nullptr;
Light* Object3D::light = nullptr;
ComPtr <ID3D12DescriptorHeap> Object3D::descHeapSRV = nullptr;
ComPtr <ID3D12DescriptorHeap> Object3D::descHeapDepthSRV = nullptr;
ComPtr<ID3D12RootSignature> Object3D::fbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::fbxPipelinestate[PIPELINE_COUNT] = {};
ComPtr<ID3D12RootSignature> Object3D::shadowFbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::shadowFbxPipelinestate = {};
ComPtr<ID3D12RootSignature> Object3D::objRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::objPipelinestate[PIPELINE_COUNT] = {};
ComPtr<ID3D12RootSignature> Object3D::shadowObjRootsignature = nullptr;
ComPtr<ID3D12PipelineState> Object3D::shadowObjPipelinestate = {};
int Object3D::prevDrawObjectType = -1;
int Object3D::prevPipelineIndex = -1;
std::vector<Object3D*> Object3DManager::pObject3DList;
int Object3D::loadCount = 0;
XMMATRIX Object3D::matOrthographicLH = XMMatrixOrthographicLH(1280 * 0.5f, 720 * 0.5f, 1, 100);

void Object3D::StaticInitialize()
{
	HRESULT result;

	//SRV用デスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	descHeapDesc.NumDescriptors = 1024;//テクスチャ枚数
	result = DX12Util::GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(descHeapSRV.ReleaseAndGetAddressOf()));//生成

	assert(SUCCEEDED(result));

	//深度テクスチャ用SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resDesc.Texture2D.MipLevels = 1;
	resDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	resDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descHeapSRV->GetCPUDescriptorHandleForHeapStart();

	DX12Util::GetDevice()->CreateShaderResourceView(DX12Util::GetDepthBuffer(1), &resDesc, handle);
	loadCount++;
}

void Object3D::CreateGraphicsPipeline(int objectType, PipelineData& pipelineData)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob;	//ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	assert(device);

	// 頂点シェーダの読み込みとコンパイル
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(pipelineData.vertexShaderFileName), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(pipelineData.vertexShaderFileName));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // インクルード可能にする
				"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
				0,
				&vsBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchiveVS = true;
			}
		}
	}

	if (isLoadedArchiveVS == false) {
		result = D3DCompileFromFile(
			pipelineData.vertexShaderFileName,    // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ジオメトリシェーダの読み込みとコンパイル
	if (pipelineData.geometryShaderFileName != nullptr) {
		bool isLoadedArchiveGS = false;
		if (Archive::IsOpenArchive()) {
			int size;
			void* pData = Archive::GetPData(Encorder::WstrToStr(pipelineData.geometryShaderFileName), &size);
			std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(pipelineData.geometryShaderFileName));

			if (pData != nullptr) {

				result = D3DCompile(
					mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
					nullptr,
					nullptr, // インクルード可能にする
					"main", "gs_5_0",    // エントリーポイント名、シェーダーモデル指定
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
					0,
					&gsBlob, &errorBlob);

				if (result == S_OK) {
					isLoadedArchiveGS = true;
				}
			}
		}

		if (isLoadedArchiveGS == false) {
			result = D3DCompileFromFile(
				pipelineData.geometryShaderFileName,    // シェーダファイル名
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
				"main", "gs_5_0",    // エントリーポイント名、シェーダーモデル指定
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
				0,
				&gsBlob, &errorBlob);
		}

		if (FAILED(result)) {
			// errorBlobからエラー内容をstring型にコピー
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += "\n";
			// エラー内容を出力ウィンドウに表示
			OutputDebugStringA(errstr.c_str());
			exit(1);
		}
	}

	// ピクセルシェーダの読み込みとコンパイル
	bool isLoadedArchivePS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(pipelineData.pixelShaderFileName), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(pipelineData.pixelShaderFileName));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // インクルード可能にする
				"main", "ps_5_0",    // エントリーポイント名、シェーダーモデル指定
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
				0,
				&psBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchivePS = true;
			}
		}
	}

	if (isLoadedArchivePS == false) {
		result = D3DCompileFromFile(
			pipelineData.pixelShaderFileName,    // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0",    // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//// 頂点レイアウト
	//D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	//	{ // xy座標(1行で書いたほうが見やすい)
	//		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // 法線ベクトル(1行で書いたほうが見やすい)
	//		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv座標(1行で書いたほうが見やすい)
	//		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv座標(1行で書いたほうが見やすい)
	//		"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//	{ // uv座標(1行で書いたほうが見やすい)
	//		"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	//	},
	//};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	if (pipelineData.geometryShaderFileName != nullptr) {
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	}
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	
	//透明部分の深度値書き込み設定
	gpipeline.BlendState.AlphaToCoverageEnable = false;

	// デプスステンシルステート
	//標準的な設定(深度テストを行う、書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = pipelineData.inputLayout.data();
	gpipeline.InputLayout.NumElements = pipelineData.inputLayout.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 2;    // 描画対象は3つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA

	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc[2];
	//通常用
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].MaxAnisotropy = 0;
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[0].MinLOD = 0;
	samplerDesc[0].MipLODBias = 0.0f;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].RegisterSpace = 0;

	//影用
	samplerDesc[1] = samplerDesc[0];
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//比較結果をバイリニア補間
	samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplerDesc[1].ShaderRegister = 1;	//t1

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(pipelineData.rootparams.size(), pipelineData.rootparams.data(), _countof(samplerDesc), samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	if (objectType == ObjectType::OBJECTTYPE_OBJ) {
		// ルートシグネチャの生成
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(objRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = objRootsignature.Get();
	}
	else if (objectType == ObjectType::OBJECTTYPE_FBX) {
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(fbxRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = fbxRootsignature.Get();
	}

	for (int i = 0; i < PIPELINE_COUNT; i++) {
		//ブレンドステートの設定
		// レンダーターゲットのブレンド設定(8 個あるがいまは一つしか使わない)
		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定

		blenddesc.BlendEnable = true; // ブレンドを有効にする

		blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // 加算

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う

		switch (i) {
		case OBJECT3D_BLENDMODE_NORMAL:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case OBJECT3D_BLENDMODE_ADD:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case OBJECT3D_BLENDMODE_SUB:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		case OBJECT3D_BLENDMODE_MUL:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		case OBJECT3D_BLENDMODE_SCREEN:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case OBJECT3D_BLENDMODE_REVERSE:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		}

		// ブレンドステートに設定する
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		gpipeline.BlendState.RenderTarget[1] = blenddesc;

		if (objectType == ObjectType::OBJECTTYPE_OBJ) {
			// グラフィックスパイプラインの生成
			result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(objPipelinestate[i].ReleaseAndGetAddressOf()));
			if (FAILED(result)) { assert(0); }
		}
		else if (objectType == ObjectType::OBJECTTYPE_FBX) {
			// グラフィックスパイプラインの生成
			result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(fbxPipelinestate[i].ReleaseAndGetAddressOf()));
			if (FAILED(result)) { assert(0); }
		}
	}

}

void Object3D::CreateShadowObjGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/OBJShadowTestVS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/OBJShadowTestVS.hlsl"));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // インクルード可能にする
				"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
				0,
				&vsBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchiveVS = true;
			}
		}
	}

	if (isLoadedArchiveVS == false) {
		result = D3DCompileFromFile(
			L"Shader/OBJShadowTestVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);
	}


	if (FAILED(result)) {
		// errorBlob からエラー内容を string 型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	//頂点レイアウト配列の宣言と、各種項目の設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//グラフィックスパイプラインの各ステージの設定をする構造体を用意する。
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//頂点シェーダ、ピクセルシェーダをパイプラインに設定
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS.BytecodeLength = 0;
	gpipeline.PS.pShaderBytecode = nullptr;

	//サンプルマスクとラスタライザステートの設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	//ラスタライズステート
	//標準的な設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//一旦標準値をセット
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;				//カリングしない

	//デプスステンシルステートの設定
	//標準的な設定(深度テストを行う、書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//透明部分の深度値書き込み設定
	gpipeline.BlendState.AlphaToCoverageEnable = false;


	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


	//その他の設定
	gpipeline.NumRenderTargets = 0; // 描画対象は 1 つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_UNKNOWN; // 0~255 指定の RGBA

	gpipeline.SampleDesc.Count = 1; // 1 ピクセルにつき 1 回サンプリング

	//デスクリプタテーブルの設定
	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //t2 レジスタ

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2];
	//定数用
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)
	//テクスチャ用
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//サンプラーの設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	//samplerDesc.MaxAnisotropy = 1;

	//ルートシグネチャの生成
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr <ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	//ルートシグネチャの生成
	result = DX12Util::GetDevice()->CreateRootSignature(0, rootSigBlob.Get()->GetBufferPointer(), rootSigBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(shadowObjRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = shadowObjRootsignature.Get();

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowObjPipelinestate.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
	
}

void Object3D::CreateShadowFbxGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
		// 頂点シェーダの読み込みとコンパイル
	bool isLoadedArchiveVS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/FBXShadowTestVS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/FBXShadowTestVS.hlsl"));

		if (pData != nullptr) {

			result = D3DCompile(
				mergedHlsl.c_str(), mergedHlsl.size(), nullptr,
				nullptr,
				nullptr, // インクルード可能にする
				"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
				0,
				&vsBlob, &errorBlob);

			if (result == S_OK) {
				isLoadedArchiveVS = true;
			}
		}
	}

	if (isLoadedArchiveVS == false) {
		result = D3DCompileFromFile(
			L"Shader/FBXShadowTestVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);
	}

	if (FAILED(result)) {
		// errorBlob からエラー内容を string 型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	//頂点レイアウト配列の宣言と、各種項目の設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // スキンインデックス
			"BONEINDICES",
			0, 
			DXGI_FORMAT_R32G32B32A32_UINT, 
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,

			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
			0
		},
		{ // スキンウェイト
			"BONEWEIGHTS", 
			0, 
			DXGI_FORMAT_R32G32B32A32_FLOAT, 
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
			0
		},
	};

	//グラフィックスパイプラインの各ステージの設定をする構造体を用意する。
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//頂点シェーダ、ピクセルシェーダをパイプラインに設定
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS.BytecodeLength = 0;
	gpipeline.PS.pShaderBytecode = nullptr;

	//サンプルマスクとラスタライザステートの設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	//ラスタライズステート
	//標準的な設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//一旦標準値をセット
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;				//カリングしない

	//デプスステンシルステートの設定
	//標準的な設定(深度テストを行う、書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//透明部分の深度値書き込み設定
	gpipeline.BlendState.AlphaToCoverageEnable = false;

	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


	//その他の設定
	gpipeline.NumRenderTargets = 0; // 描画対象は 1 つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_UNKNOWN; // 0~255 指定の RGBA

	gpipeline.SampleDesc.Count = 1; // 1 ピクセルにつき 1 回サンプリング

	//デスクリプタテーブルの設定
	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //t2 レジスタ

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[3];
	//定数用
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)
	//テクスチャ用
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//FBXスキニング
	rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);


	//サンプラーの設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	//samplerDesc.MaxAnisotropy = 1;

	//ルートシグネチャの生成
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr <ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	//ルートシグネチャの生成
	result = DX12Util::GetDevice()->CreateRootSignature(0, rootSigBlob.Get()->GetBufferPointer(), rootSigBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(shadowFbxRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = shadowFbxRootsignature.Get();

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowFbxPipelinestate.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
}

Object3D::~Object3D()
{
	if (collider) {
		//コリジョンマネージャから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

void Object3D::Initialize()
{
	HRESULT result;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataShare) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffShare.ReleaseAndGetAddressOf())
	);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffTransform.ReleaseAndGetAddressOf())
	);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataShadow) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffShadow.ReleaseAndGetAddressOf())
	);
	
	////オブジェクトマネージャーに追加
	//if (addManager == true) {
	//	Object3DManager::AddObject(this);
	//}

	//クラス名の文字列を取得
	name = typeid(*this).name();
}

void Object3D::Update()
{
	HRESULT result;

	XMMATRIX matScale, matRot, matTrans;

	//スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//ワールド行列の合成
	matWorld = XMMatrixIdentity();	//変形をリセット
	matWorld *= matScale;			//ワールド行列にスケーリングを反映
	matWorld *= matRot;				//ワールド行列に回転を反映
	matWorld *= matTrans;			//ワールド行列に平行移動を反映

	//ビュープロジェクション行列
	XMMATRIX matViewProjection = camera->GetViewProjection();

	//ライト情報
	Vector3 target = light->GetLightTarget();
	Vector3 up = { 0,1,0 };

	//ライトのビュー行列
	XMMATRIX matLight = XMMatrixIdentity();
	XMMATRIX matLightView = XMMatrixLookAtLH(
		XMLoadFloat3(&light->GetLightPos()), XMLoadFloat3(&target), XMLoadFloat3(&up)
	);
	//平行投影
	XMMATRIX matLightProj = matOrthographicLH;

	//行列を合成
	matLight = matLightView * matLightProj;


	if (objectType == OBJECTTYPE_OBJ) {

		//定数バッファへデータ転送
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->lightCamera = matLight;
			constMap->isDrawShadowToMyself = isDrawShadowToMyself;
			constMap->cameraPos = camera->GetPosition();
			constBuffShare->Unmap(0, nullptr);
		}

		ConstBufferDataTransform* constMap2 = nullptr;
		result = constBuffTransform->Map(0, nullptr, (void**)&constMap2);
		if (SUCCEEDED(result)) {
			constMap2->data.color = color;
			constMap2->data.world = matWorld;
			constBuffTransform->Unmap(0, nullptr);
		}
	}
	else if (objectType == OBJECTTYPE_FBX) {

		//モデルのメッシュトランスフォーム
		XMMATRIX modelTransform = fbxModel->GetModelTransForm();


		//定数バッファへデータ転送
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->lightCamera = matLight;
			constMap->cameraPos = camera->GetPosition();
			constBuffShare->Unmap(0, nullptr);
		}

		ConstBufferDataTransform* constMap2 = nullptr;
		result = constBuffTransform->Map(0, nullptr, (void**)&constMap2);
		if (SUCCEEDED(result)) {
			constMap2->data.color = color;
			constMap2->data.world = modelTransform * matWorld;
			constBuffTransform->Unmap(0, nullptr);
		}

		//アニメーション
		if (isPlay) {
			//1フレーム進める
			currentTime += frameTime;
			//最後まで再生したら先頭に戻す
			if (currentTime > endTime) {
				currentTime = startTime;
			}
		}

		//ボーン配列
		std::vector<FbxModel::Bone>& bones = fbxModel->GetBones();

		//定数バッファへデータ転送
		ConstBufferDataSkin* constMapSkin = nullptr;
		result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
		for (int i = 0; i < bones.size(); i++) {
			//今の姿勢行列
			XMMATRIX matCurrentPose;
			//今の姿勢行列を取得
			FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
			//XMMATRIXに変換
			FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
			//合成してスキニング行列に
			constMapSkin->bones[i] = fbxModel->GetModelTransForm() * bones[i].invInitialPose * matCurrentPose * fbxModel->GetModelTransForm();
		}
		constBuffSkin->Unmap(0, nullptr);

	}

	ConstBufferDataShadow* constMap3 = nullptr;
	result = constBuffShadow->Map(0, nullptr, (void**)&constMap3);
	if (SUCCEEDED(result)) {
		constMap3->lightCamera = matLight;
		constMap3->world = matWorld;
		constBuffShadow->Unmap(0, nullptr);
	}


	//当たり判定更新
	if (collider) {
		collider->Update();
	}
}

void Object3D::BeginDraw()
{
	//デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//プリミティブ形状を設定
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	prevDrawObjectType = -1;
	prevPipelineIndex = -1;
}

void Object3D::DrawPrivate()
{
	ConstBufferDataTransform* constMap = nullptr;

	if (objectType == OBJECTTYPE_OBJ) {
		//モデルの割り当てがなければ描画しない
		if (objModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//ルートシグネチャの設定
			DX12Util::GetCmdList()->SetGraphicsRootSignature(objRootsignature.Get());
			//パイプラインステートの設定
			DX12Util::GetCmdList()->SetPipelineState(objPipelinestate[pipelineIndex].Get());
		}
		//前回の描画に使用されたパイプラインのインデックスと今回使うものが違うなら
		//ここで切り替え
		else if (pipelineIndex != prevPipelineIndex) {
			//パイプラインステートの設定
			DX12Util::GetCmdList()->SetPipelineState(objPipelinestate[pipelineIndex].Get());
		}
		prevPipelineIndex = pipelineIndex;

		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		// シェーダリソースビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootDescriptorTable(5,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
				0,
				DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
		//ライトの描画
		light->Draw(3);
		//モデル描画
		if (objModel != nullptr) {
			objModel->Draw();
		}

	}
	else if (objectType == OBJECTTYPE_FBX) {
		//モデルの割り当てがなければ描画しない
		if (fbxModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//ルートシグネチャの設定
			DX12Util::GetCmdList()->SetGraphicsRootSignature(fbxRootsignature.Get());
		}

		//前回の描画に使用されたパイプラインのインデックスと今回使うものが違うなら
		//ここで切り替え
		static int prevPipelineIndex = -1;
		if (pipelineIndex != prevPipelineIndex) {
			//パイプラインステートの設定
			DX12Util::GetCmdList()->SetPipelineState(fbxPipelinestate[pipelineIndex].Get());
		}
		prevPipelineIndex = pipelineIndex;

		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransform->GetGPUVirtualAddress());
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
		// シェーダリソースビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootDescriptorTable(7,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetGPUDescriptorHandleForHeapStart(),
				0,
				DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
		//ライトの描画
		light->Draw(3);
		//モデル描画
		fbxModel->Draw();
	}

	prevDrawObjectType = objectType;
}

void Object3D::BeginDrawShadow()
{
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = DX12Util::GetDepthHeap()->GetCPUDescriptorHandleForHeapStart();
	dsvH.ptr += DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//レンダーターゲットをセット
	DX12Util::GetCmdList()->OMSetRenderTargets(0, nullptr, false, &dsvH);

	//4.ビューポートの設定コマンド
	DX12Util::GetCmdList()->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight()));
	DX12Util::GetCmdList()->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight()));

	//5.シザー矩形の設定コマンド
	D3D12_RECT scissorrect{};
	scissorrect.left = 0; // 切り抜き座標左
	scissorrect.right = scissorrect.left + DX12Util::GetWindowWidth(); // 切り抜き座標右
	scissorrect.top = 0; // 切り抜き座標上
	scissorrect.bottom = scissorrect.top + DX12Util::GetWindowHeight(); // 切り抜き座標下
	DX12Util::GetCmdList()->RSSetScissorRects(1, &scissorrect);

	////デスクリプタヒープのセット
	//ID3D12DescriptorHeap* ppHeaps[] = { descHeapDepthSRV.Get() };
	//DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//プリミティブ形状を設定
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//背景描画ここまで
	DX12Util::ClearDepthBuffer(true);

	prevDrawObjectType = -1;
	prevPipelineIndex = -1;
}

void Object3D::DrawShadow()
{
	if (isDrawShadowToOther == false) {
		return;
	}

	if (objectType == OBJECTTYPE_OBJ) {
		//モデルの割り当てがなければ描画しない
		if (objModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//ルートシグネチャの設定
			DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowObjRootsignature.Get());

			//パイプラインステートの設定
			DX12Util::GetCmdList()->SetPipelineState(shadowObjPipelinestate.Get());
		}
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadow->GetGPUVirtualAddress());

		//モデル描画
		if (objModel != nullptr) {
			objModel->Draw(1, true);
		}

	}
	else if (objectType == OBJECTTYPE_FBX) {
		//モデルの割り当てがなければ描画しない
		if (fbxModel == nullptr) {
			return;
		}

		if (objectType != prevDrawObjectType) {
			//ルートシグネチャの設定
			DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowFbxRootsignature.Get());

			//パイプラインステートの設定
			DX12Util::GetCmdList()->SetPipelineState(shadowFbxPipelinestate.Get());
		}
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadow->GetGPUVirtualAddress());
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());
		//モデル描画
		fbxModel->Draw(1, true);
	}
}

void Object3D::Draw()
{
	DrawManager::AddDrawList(DRAW_MANAGER_OBJECT_TYPE_OBJECT3D, this);
}

void Object3D::PlayAnimation()
{
	if (objectType == OBJECTTYPE_OBJ || objectType == OBJECTTYPE_INSTANCING_OBJ) {
		return;
	}

	FbxScene* fbxScene = fbxModel->GetFbxScene();
	//0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//アニメーションの名前取得
	const char* animstackname = animstack->GetName();
	//アニメーションの時間取得
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//開始時間取得
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	//終了時間取得
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	//開始時間に合わせる
	currentTime = startTime;
	//再生中状態にする
	isPlay = true;
}

void Object3D::SetFbxModel(FbxModel* fbxModel)
{
	this->fbxModel = fbxModel;

	//定数バッファの生成
	HRESULT result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffSkin.ReleaseAndGetAddressOf())
	);

	//定数バッファへデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	if (SUCCEEDED(result)) {
		for (int i = 0; i < MAX_BONES; i++) {
			constMapSkin->bones[i] = XMMatrixIdentity();
		}
		constBuffSkin->Unmap(0, nullptr);
	}

	//1フレーム分の時間を60FPSで設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	objectType = OBJECTTYPE_FBX;
}

void Object3D::SetObjModel(ObjModel* objModel)
{
	this->objModel = objModel;

	objectType = OBJECTTYPE_OBJ;
}

void Object3D::SetCollider(BaseCollider* collider)
{
	//既に登録されている場合はRemove
	if (this->collider) {
		CollisionManager::GetInstance()->RemoveCollider(this->collider);
	}

	collider->SetObject(this);
	this->collider = collider;
	//コリジョンマネージャに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//コライダーを更新しておく
	collider->Update();
}

void Object3DManager::Initialize()
{
	pObject3DList.clear();
}

void Object3DManager::AddObject(Object3D* p)
{
	pObject3DList.emplace_back(p);
}

void Object3DManager::UpdateAllObject()
{
	for (int i = 0; i < pObject3DList.size(); i++) {
		if (pObject3DList[i] != nullptr) {
			pObject3DList[i]->Update();
		}
	}
}

void Object3DManager::DeleteObject(Object3D* p)
{
	if (p == nullptr) {
		return;
	}

	auto itr = std::find(pObject3DList.begin(), pObject3DList.end(), p);
	if (itr != pObject3DList.end()) {
		pObject3DList.erase(itr);
	}
}

void Object3DManager::DeleteAllObject()
{
	pObject3DList.clear();
}
