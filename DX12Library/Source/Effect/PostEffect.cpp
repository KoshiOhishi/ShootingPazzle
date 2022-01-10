#include "PostEffect.h"
#include "DX12Util.h"
#include "Input.h"

#include <d3dx12.h>
#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
///


PostEffect::ComPtr<ID3D12Resource> PostEffect::texBuff[RENDERCOUNT];
PostEffect::ComPtr<ID3D12DescriptorHeap> PostEffect::descHeapRTV;

//										  Red	Green  Blue Alpha
const float PostEffect::clearColor[4] = { 0.25f, 0.5f, 0.1f, 0.0f }; //緑っぽい色

using namespace DirectX;

void PostEffect::Initialize(bool isDispDepthTex)
{
	HRESULT result;


	//VBV作成
	CreateVBV();
	//定数バッファ生成
	CreateConstBuffer();
	//テクスチャ生成
	CreateTexture();
	//SRV作成
	CreateSRV();
	//RTV作成
	CreateRTV();
	//パイプライン生成
	CreateGraphicsPipelineState(isDispDepthTex);
}

void PostEffect::PreDrawScene()
{
	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();
	for (int i = 0; i < RENDERCOUNT; i++) {

		//リソースバリア変更（シェーダーリソース→描画可能）
		cmdList->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET)
		);
	}

	//レンダーターゲットビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[RENDERCOUNT];
	for (int i = 0; i < RENDERCOUNT; i++) {
		rtvHs[i] = GetRtvH(i);
	}

	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = DX12Util::GetDepthHeap()->GetCPUDescriptorHandleForHeapStart();

	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(RENDERCOUNT, rtvHs, true, &dsvH);

	CD3DX12_VIEWPORT viewports[RENDERCOUNT];
	CD3DX12_RECT scissorRects[RENDERCOUNT];
	for (int i = 0; i < RENDERCOUNT; i++) {
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight());
		scissorRects[i] = CD3DX12_RECT(0, 0, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight());
	}

	//ビューポートの設定
	cmdList->RSSetViewports(RENDERCOUNT, viewports);

	//シザリング短形の設定
	cmdList->RSSetScissorRects(RENDERCOUNT, scissorRects);

	for (int i = 0; i < RENDERCOUNT; i++) {
		//全画面クリア
		cmdList->ClearRenderTargetView(rtvHs[i], clearColor, 0, nullptr);
	}

	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene()
{
	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();

	for (int i = 0; i < RENDERCOUNT; i++) {
		//リソースバリア変更（描画可能→シェーダーリソース）
		cmdList->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		);
	}

}

void PostEffect::Draw()
{
	if (!isDisplay)
		return;

	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//ワールド行列の更新
	matWorld = XMMatrixIdentity();
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));
	matWorld *= XMMatrixTranslation(position.x, position.y, 0);

	//行列の転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->mat = XMMatrixIdentity();
	constBuff->Unmap(0, nullptr);

	//時間の転送
	ConstBufferDataTime* constMapTime = nullptr;
	result = constBuffTime->Map(0, nullptr, (void**)&constMapTime);
	constMapTime->time = time / 1000;
	constBuffTime->Unmap(0, nullptr);


	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get()};
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(1, constBuffTime->GetGPUVirtualAddress());

	//シェーダリソースビューをセット(テクスチャ1枚目)
	cmdList->SetGraphicsRootDescriptorTable(2, 
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 0,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//シェーダリソースビューをセット(テクスチャ2枚目)
	cmdList->SetGraphicsRootDescriptorTable(3,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 1,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//シェーダリソースビューをセット(深度テクスチャ)
	cmdList->SetGraphicsRootDescriptorTable(4,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 2,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);

}

void PostEffect::CreateVBV()
{
	HRESULT result;

	//頂点バッファ生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//頂点データ
	float sizeMag = 1.0f;
	VertexPosUv vertices[] = {
		//	 x		y	  z		u	  v
		{{-sizeMag, -sizeMag, 0.0f},{0.0f,1.0f}},	//左下
		{{-sizeMag, +sizeMag, 0.0f},{0.0f,0.0f}},	//左上
		{{+sizeMag, -sizeMag, 0.0f},{1.0f,1.0f}},	//右下
		{{+sizeMag, +sizeMag, 0.0f},{1.0f,0.0f}},	//右上

	};

	//頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * vertNum;
	vbView.StrideInBytes = sizeof(VertexPosUv);

}

void PostEffect::CreateConstBuffer()
{
	HRESULT result;

	//定数バッファの生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//定数バッファの生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTime) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffTime.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));
}

void PostEffect::CreateTexture()
{
	HRESULT result;

	//テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DX12Util::GetWindowWidth(),
		(UINT)DX12Util::GetWindowHeight(),
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	auto clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor);

	//テクスチャバッファの生成
	for (int i = 0; i < RENDERCOUNT; i++) {
		result = DX12Util::GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(texBuff[i].ReleaseAndGetAddressOf())
		);

		assert(SUCCEEDED(result));

		{	//テクスチャを赤クリア
			//画素数(1280  x 720 = 921600)ピクセル
			const UINT pixelCount = DX12Util::GetWindowWidth() * DX12Util::GetWindowHeight();
			//画像1行分のデータサイズ
			const UINT rowPitch = sizeof(UINT) * DX12Util::GetWindowWidth();
			//画像全体のデータサイズ
			const UINT depthPitch = rowPitch * DX12Util::GetWindowHeight();
			//画像イメージ
			UINT* img = new UINT[pixelCount];
			for (int j = 0; j < pixelCount; j++) { img[j] = 0xff0000ff; }

			//テクスチャバッファにデータ転送
			result = texBuff[i]->WriteToSubresource(0, nullptr,
				img, rowPitch, depthPitch
			);
			assert(SUCCEEDED(result));
			delete[] img;
		}
	}

}

void PostEffect::CreateSRV()
{
	HRESULT result;

	//SRV用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	srvDescHeapDesc.NumDescriptors = RENDERCOUNT + 1;//テクスチャ枚数
	result = DX12Util::GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(descHeapSRV.ReleaseAndGetAddressOf()));//生成
	assert(SUCCEEDED(result));

	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; //設定構造体
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < RENDERCOUNT; i++) {
		//デスクリプタヒープにSRV作成
		DX12Util::GetDevice()->CreateShaderResourceView(texBuff[i].Get(), //ビューと関連付けるバッファ
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), i,
				DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}
	
	//深度テクスチャ用SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resDesc.Texture2D.MipLevels = 1;
	resDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	resDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

	DX12Util::GetDevice()->CreateShaderResourceView(DX12Util::GetDepthBuffer(1), &resDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetCPUDescriptorHandleForHeapStart(), RENDERCOUNT,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
}

void PostEffect::CreateRTV()
{
	HRESULT result;

	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = RENDERCOUNT;
	//RTV用デスクリプタヒープを作成
	result = DX12Util::GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(descHeapRTV.ReleaseAndGetAddressOf()));//生成
	assert(SUCCEEDED(result));

	for (int i = 0; i < RENDERCOUNT; i++) {
		//デスクリプタヒープにRTV作成
		DX12Util::GetDevice()->CreateRenderTargetView(texBuff[i].Get(),
			nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
				DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			)
		);
	}
}

void PostEffect::CreateGraphicsPipelineState(bool isDispDepthTex)
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/PostEffectTestVS.hlsl", // シェーダファイル名
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

	LPCSTR entry = "main";
	if (isDispDepthTex) { entry = "main_depth"; }

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/PostEffectTestPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		entry, "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);


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

		{"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0},

	};

	//グラフィックスパイプラインの各ステージの設定をする構造体を用意する。
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//頂点シェーダ、ピクセルシェーダをパイプラインに設定
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());


	//サンプルマスクとラスタライザステートの設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	//ラスタライズステート
	//標準的な設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);		//一旦標準値をセット
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;				//カリングしない

	//デプスステンシルステートの設定
	//標準的な設定(深度テストを行う、書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	//一旦標準値をセット
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//常に上書きルール
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;


	//ブレンドステートの設定
	// レンダーターゲットのブレンド設定(8 個あるがいまは一つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定
	blenddesc.BlendEnable = true; // ブレンドを有効にする

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1.0f-ソースのアルファ値

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う

	////透明部分の深度値書き込み禁止
	//gpipeline.BlendState.AlphaToCoverageEnable = true;

	// ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


	//その他の設定
	gpipeline.NumRenderTargets = 1; // 描画対象は 1 つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255 指定の RGBA

	gpipeline.SampleDesc.Count = 1; // 1 ピクセルにつき 1 回サンプリング

	//デスクリプタテーブルの設定
	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0 レジスタ

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //t1 レジスタ

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
	descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //t2 レジスタ

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[5];
	//定数用
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)
	//定数用(時間)
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b1)
	//テクスチャ用
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);

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
		IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootSignature.Get();

	//パイプラインステートの生成
	result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
}

D3D12_CPU_DESCRIPTOR_HANDLE PostEffect::GetRtvH(int targetNum)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descHeapRTV->GetCPUDescriptorHandleForHeapStart(), targetNum,
		DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	);
}
