#include "Line3D.h"
#include "DrawManager.h"
#include "Archive.h"
#include "Encorder.h"
#include <DX12Util.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DX12Library;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Line3D::device = nullptr;
Camera* Line3D::camera = nullptr;
ComPtr<ID3D12RootSignature> Line3D::rootsignature = nullptr;
ComPtr<ID3D12PipelineState> Line3D::pipelinestate = nullptr;
ComPtr<ID3D12Resource> Line3D::constBuff = nullptr;
ComPtr<ID3D12Resource> Line3D::vertBuff = nullptr;
ComPtr<ID3D12Resource> Line3D::indexBuff = nullptr;
std::vector<LineInfo> Line3D::lines;

void Line3D::DrawAll()
{
	HRESULT result;

	//頂点バッファへのデータ転送
	// GPU 上のバッファに対応した仮想メモリを取得
	VertexLine3D* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		for (int i = 0; i < lines.size(); i++) {
			vertMap[i * 2].pos = lines[i].startPos;
			vertMap[i * 2].color = lines[i].startColor;
			vertMap[i * 2 + 1].pos = lines[i].endPos;
			vertMap[i * 2 + 1].color = lines[i].endColor;
		}
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView;
	vbView.BufferLocation = vertBuff.Get()->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexLine3D) * 2 * lines.size();
	vbView.StrideInBytes = sizeof(VertexLine3D);

	//ルートシグネチャの設定
	DX12Util::GetCmdList()->SetGraphicsRootSignature(rootsignature.Get());
	//パイプラインステートの設定
	DX12Util::GetCmdList()->SetPipelineState(pipelinestate.Get());

	//定数バッファビューをセット
	DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	////プリミティブ形状の設定コマンド(三角形リスト)
	//DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファの設定コマンド
	DX12Util::GetCmdList()->IASetVertexBuffers(0, 1, &vbView);	

	//描画コマンド
	DX12Util::GetCmdList()->DrawInstanced(lines.size() * 2, 1, 0, 0);

}

void Line3D::StaticInitialize()
{
	device = DX12Util::GetDevice();

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
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/LineVS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/LineVS.hlsl"));

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
			L"Shader/LineVS.hlsl",    // シェーダファイル名
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

	// ピクセルシェーダの読み込みとコンパイル
	bool isLoadedArchivePS = false;
	if (Archive::IsOpenArchive()) {
		int size;
		void* pData = Archive::GetPData(Encorder::WstrToStr(L"Shader/LinePS.hlsl"), &size);
		std::string mergedHlsl = Encorder::GetMergedHLSLI(pData, size, Encorder::WstrToStr(L"Shader/LinePS.hlsl"));

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
			L"Shader/LinePS.hlsl",    // シェーダファイル名
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

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // xy座標(1行で書いたほうが見やすい)
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		//{ // 法線ベクトル(1行で書いたほうが見やすい)
		//	"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		//	D3D12_APPEND_ALIGNED_ELEMENT,
		//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		//},
		//{ // uv座標(1行で書いたほうが見やすい)
		//	"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		//	D3D12_APPEND_ALIGNED_ELEMENT,
		//	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		//},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
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
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（線）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	gpipeline.NumRenderTargets = 1;    // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA

	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[1];

	//定数用
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc[1];
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

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, _countof(samplerDesc), samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	// ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = rootsignature.Get();

	//ブレンドステートの設定
	// レンダーターゲットのブレンド設定(8 個あるがいまは一つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定

	blenddesc.BlendEnable = true; // ブレンドを有効にする

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // 加算

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う

	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	// ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate.ReleaseAndGetAddressOf()));


	if (FAILED(result)) { assert(0); }

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);

	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * 2 * MAX_DRAW_COUNT);

	//頂点バッファの確保
	//GPU 側のメモリを確保して、頂点データ用の領域を設定。
	//頂点バッファ用 GPU リソースの生成
	// 頂点バッファの生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB), // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf())
	);


}

void Line3D::Reset()
{
	lines.clear();
}

void Line3D::BeginDraw()
{
	//デスクリプタヒープをセット
	//ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	//DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//プリミティブ形状を設定
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	//定数バッファにデータ転送
	ConstBuffData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->color = { 1,1,1,1 };
		constMap->matWorld = camera->GetViewProjection();
		constBuff->Unmap(0, nullptr);
	}
}

void Line3D::DrawLine(const Vector3& start, const Vector3& end)
{
	//線描画リストに追加 (白固定)
	lines.emplace_back(LineInfo{ start,end,{1,1,1,1},{1,1,1,1} });
}

void Line3D::DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector4& color)
{
	//線描画リストに追加
	lines.emplace_back(LineInfo{ startPos, endPos, color, color });
}

void Line3D::DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector4& startColor, const Vector4& endColor)
{
	//線描画リストに追加
	lines.emplace_back(LineInfo{ startPos, endPos, startColor, endColor });
}