#include "InstancingObject.h"
#include "BaseCollider.h"

using namespace DirectX;
using namespace Microsoft::WRL;

ComPtr<ID3D12RootSignature> InstancingObjectDraw::instancingFbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::instancingFbxPipelinestate = nullptr;
ComPtr<ID3D12RootSignature> InstancingObjectDraw::shadowInstancingFbxRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::shadowInstancingFbxPipelinestate = nullptr;
ComPtr<ID3D12RootSignature> InstancingObjectDraw::instancingObjRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::instancingObjPipelinestate = nullptr;
ComPtr<ID3D12RootSignature> InstancingObjectDraw::shadowInstancingObjRootsignature = nullptr;
ComPtr<ID3D12PipelineState> InstancingObjectDraw::shadowInstancingObjPipelinestate = nullptr;

void InstancingObjectDraw::CreateGraphicsPipeline(int objectType, PipelineData& pipelineData)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob;	//ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	assert(device);

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		pipelineData.vertexShaderFileName,    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
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
		result = D3DCompileFromFile(
			pipelineData.geometryShaderFileName,    // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "gs_5_0",    // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&gsBlob, &errorBlob);
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
	result = D3DCompileFromFile(
		pipelineData.pixelShaderFileName,    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",    // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
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
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	// デプスステンシルステート
	//標準的な設定(深度テストを行う、書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;


	//透明部分の深度値書き込み禁止
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = pipelineData.inputLayout.data();
	gpipeline.InputLayout.NumElements = pipelineData.inputLayout.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 2;    // 描画対象は2つ
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

	if (objectType == ObjectType::OBJECTTYPE_INSTANCING_OBJ) {
		// ルートシグネチャの生成
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(instancingObjRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = instancingObjRootsignature.Get();

		// グラフィックスパイプラインの生成
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(instancingObjPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

	}
	else if (objectType == ObjectType::OBJECTTYPE_INSTANCING_FBX) {
		result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(instancingFbxRootsignature.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }

		gpipeline.pRootSignature = instancingFbxRootsignature.Get();

		// グラフィックスパイプラインの生成
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(instancingFbxPipelinestate.ReleaseAndGetAddressOf()));
		if (FAILED(result)) { assert(0); }
	}

}

void InstancingObjectDraw::CreateShadowObjGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/InstancingOBJShadowTestVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);


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
	//ブレンドステートの設定
	// レンダーターゲットのブレンド設定(8 個あるがいまは一つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定

	blenddesc.BlendEnable = true; // ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1.0f-ソースのアルファ値

	//透明部分の深度値書き込み禁止
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// ブレンドステートに設定する
	//gpipeline.BlendState.RenderTarget[0]


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
	//定数用
	rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)

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
		IID_PPV_ARGS(shadowInstancingObjRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = shadowInstancingObjRootsignature.Get();

	//パイプラインステートの生成
	result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowInstancingObjPipelinestate.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void InstancingObjectDraw::CreateShadowFbxGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/InstancingFBXShadowTestVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);


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
	//ブレンドステートの設定
	// レンダーターゲットのブレンド設定(8 個あるがいまは一つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定

	blenddesc.BlendEnable = true; // ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1.0f-ソースのアルファ値

	//透明部分の深度値書き込み禁止
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// ブレンドステートに設定する
	//gpipeline.BlendState.RenderTarget[0]


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
	CD3DX12_ROOT_PARAMETER rootparams[4];
	//定数用
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)
	//テクスチャ用
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//定数用
	rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b1)
	//FBXスキニング
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL); //定数バッファビューとして初期化(b2)

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
		IID_PPV_ARGS(shadowInstancingFbxRootsignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = shadowInstancingFbxRootsignature.Get();

	//パイプラインステートの生成
	result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(shadowInstancingFbxPipelinestate.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

void InstancingObjectDraw::Initialize()
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

	for (int i = 0; i < _countof(constBuffTransforms); i++) {
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataInstancing) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constBuffTransforms[i].ReleaseAndGetAddressOf())
		);
	}

	for (int i = 0; i < _countof(constBuffShadows); i++) {
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataInstancingShadow) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constBuffShadows[i].ReleaseAndGetAddressOf())
		);
	}

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataInstancingLightCamera) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffLightCamera.ReleaseAndGetAddressOf())
	);

	////オブジェクトマネージャーに追加
	//if (addManager == true) {
	//	Object3DManager::AddObject(this);
	//}

	//クラス名の文字列を取得
	name = typeid(*this).name();

	datas.resize(MAX_INSTANCECOUNT);
}

void InstancingObjectDraw::Update()
{
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

	if (objectType == OBJECTTYPE_INSTANCING_OBJ) {

		HRESULT result;

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
	}
	else if (objectType == OBJECTTYPE_INSTANCING_FBX) {

		//モデルのメッシュトランスフォーム
		XMMATRIX modelTransform = fbxModel->GetModelTransForm();

		HRESULT result;

		//定数バッファへデータ転送
		ConstBufferDataShare* constMap = nullptr;
		result = constBuffShare->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {
			constMap->viewproj = matViewProjection;
			constMap->lightCamera = matLight;
			constMap->cameraPos = camera->GetPosition();;
			constBuffShare->Unmap(0, nullptr);
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

	//当たり判定更新
	if (collider) {
		collider->Update();
	}

	drawPhase = DRAW_PHASE_DRAWABLE;
}

void InstancingObjectDraw::DrawPrivate()
{
	if (drawCount == 0) {
		return;
	}

	//Update()を通らずに描画しようとしたらはじく
	if (drawPhase == DRAW_PHASE_UPDATEABLE) {
		//カウントを0に戻す
		drawCount = 0;
		return;
	}

	//定数バッファにデータ転送
	int lDrawCount = drawCount;
	int roopCount_i = lDrawCount / MAX_ONCEDRAWCOUNT + 1;
	InstancingObjectDraw::ConstBufferDataInstancing* constMap = nullptr;
	for (int i = 0; i < roopCount_i; i++) {

		HRESULT result = constBuffTransforms[i]->Map(0, nullptr, (void**)&constMap);
		if (SUCCEEDED(result)) {

			int roopCount_j = lDrawCount > 512 ? 512 : lDrawCount;
			for (int j = 0; j < roopCount_j; j++) {
				constMap->data[j].color = datas[i * MAX_ONCEDRAWCOUNT + j].color;
				constMap->data[j].world = datas[i * MAX_ONCEDRAWCOUNT + j].world;
			}

			if (objectType == OBJECTTYPE_INSTANCING_OBJ) {
				//モデルの割り当てがなければ描画しない
				if (objModel == nullptr) {
					return;
				}

				if (objectType != prevDrawObjectType) {
					//ルートシグネチャの設定
					DX12Util::GetCmdList()->SetGraphicsRootSignature(instancingObjRootsignature.Get());
					//パイプラインステートの設定
					DX12Util::GetCmdList()->SetPipelineState(instancingObjPipelinestate.Get());
				}

				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransforms[i]->GetGPUVirtualAddress());
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
				objModel->Draw(roopCount_j);

				prevDrawObjectType = objectType;

			}
			else if (objectType == OBJECTTYPE_INSTANCING_FBX) {
				//モデルの割り当てがなければ描画しない
				if (fbxModel == nullptr) {
					return;
				}
				if (objectType != prevDrawObjectType) {
					//ルートシグネチャの設定
					DX12Util::GetCmdList()->SetGraphicsRootSignature(instancingFbxRootsignature.Get());
					//パイプラインステートの設定
					DX12Util::GetCmdList()->SetPipelineState(instancingFbxPipelinestate.Get());
				}
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShare->GetGPUVirtualAddress());
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(1, constBuffTransforms[i]->GetGPUVirtualAddress());
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
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
				fbxModel->Draw(roopCount_j);

				prevDrawObjectType = objectType;
			}

			lDrawCount -= roopCount_j;
			constBuffTransforms[i]->Unmap(0, nullptr);
		}
	}

	drawCount = 0;
	drawPhase = DRAW_PHASE_UPDATEABLE;
}

void InstancingObjectDraw::DrawShadow()
{
	if (drawCount == 0 || 
		isDrawShadowToOther == false) {
		return;
	}

	//Update()を通らずに描画しようとしたらはじく
	if (drawPhase == DRAW_PHASE_UPDATEABLE) {
		return;
	}

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

	//定数バッファにデータ送信
	InstancingObjectDraw::ConstBufferDataInstancingLightCamera* constMap = nullptr;
	HRESULT result = constBuffLightCamera->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->lightCamera = matLight;
		constBuffLightCamera->Unmap(0, nullptr);
	}

	//定数バッファにデータ転送
	int lDrawCount = drawCount;
	int roopCount_i = lDrawCount / MAX_ONCEDRAWCOUNT + 1;
	InstancingObjectDraw::ConstBufferDataInstancingShadow* constMap2 = nullptr;
	for (int i = 0; i < roopCount_i; i++) {

		result = constBuffShadows[i]->Map(0, nullptr, (void**)&constMap2);
		if (SUCCEEDED(result)) {

			int roopCount_j = lDrawCount > 512 ? 512 : lDrawCount;
			for (int j = 0; j < roopCount_j; j++) {
				constMap2->world[j] = datas[i * MAX_ONCEDRAWCOUNT + j].world;
			}

			if (objectType == OBJECTTYPE_INSTANCING_OBJ) {
				//モデルの割り当てがなければ描画しない
				if (objModel == nullptr) {
					return;
				}

				if (objectType != prevDrawObjectType) {
					//ルートシグネチャの設定
					DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowInstancingObjRootsignature.Get());

					//パイプラインステートの設定
					DX12Util::GetCmdList()->SetPipelineState(shadowInstancingObjPipelinestate.Get());
				}
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadows[i]->GetGPUVirtualAddress());
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffLightCamera->GetGPUVirtualAddress());

				//モデル描画
				objModel->Draw(roopCount_j, true);

			}
			else if (objectType == OBJECTTYPE_INSTANCING_FBX) {
				//モデルの割り当てがなければ描画しない
				if (fbxModel == nullptr) {
					return;
				}

				if (objectType != prevDrawObjectType) {
					//ルートシグネチャの設定
					DX12Util::GetCmdList()->SetGraphicsRootSignature(shadowInstancingFbxRootsignature.Get());

					//パイプラインステートの設定
					DX12Util::GetCmdList()->SetPipelineState(shadowInstancingFbxPipelinestate.Get());
				}
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuffShadows[i]->GetGPUVirtualAddress());
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffLightCamera->GetGPUVirtualAddress());
				//定数バッファビューをセット
				DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(3, constBuffSkin->GetGPUVirtualAddress());
				//モデル描画
				fbxModel->Draw(roopCount_j, true);
			}

			constBuffShadows[i]->Unmap(0, nullptr);
		}
	}
}

void InstancingObjectDraw::SetFbxModel(FbxModel* fbxModel)
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

	objectType = OBJECTTYPE_INSTANCING_FBX;
}

void InstancingObjectDraw::SetObjModel(ObjModel* objModel)
{
	this->objModel = objModel;

	objectType = OBJECTTYPE_INSTANCING_OBJ;
}

void InstancingObjectDraw::AddInstancingData(const InstanceData& constBuffData)
{
	//DrawPrivate()を通過しないでデータ追加しようとしたとき、
	//そのフレームでの最初の呼び出しになるので
	//コンテナが溢れないようにdrawCountを0に戻してあげる
	if (drawPhase == DRAW_PHASE_DRAWABLE) {
		drawCount = 0;
		drawPhase = DRAW_PHASE_UPDATEABLE;
	}

	datas[drawCount] = constBuffData;
	drawCount++;
}

void InstancingObject::Initialize(const Vector3& pos, const Vector3& rot, const Vector3& scale, const Vector4& color)
{
	SetPosition(pos);
	SetRotation(rot);
	SetScale(scale);
	SetColor(color);
}

void InstancingObject::Update(InstancingObjectDraw& instancingObjectDraw)
{
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

	instancingObjectDraw.AddInstancingData({ color, matWorld });
}