#include "ParticleManager.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "DX12Util.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* ParticleManager::device = nullptr;
UINT ParticleManager::descriptorHandleIncrementSize = 0;
ID3D12GraphicsCommandList* ParticleManager::cmdList = nullptr;
ComPtr<ID3D12RootSignature> ParticleManager::rootsignature;
ComPtr<ID3D12PipelineState> ParticleManager::pipelinestate[PIPELINE_COUNT] = {};
ComPtr<ID3D12DescriptorHeap> ParticleManager::descHeap;
ComPtr<ID3D12Resource> ParticleManager::vertBuff;
//ComPtr<ID3D12Resource> ParticleManager::indexBuff;
const int ParticleManager::PATICLE_SRV_COUNT = 512;
ComPtr<ID3D12Resource> ParticleManager::texbuff[ParticleManager::PATICLE_SRV_COUNT];
CD3DX12_CPU_DESCRIPTOR_HANDLE ParticleManager::cpuDescHandleSRV;
CD3DX12_GPU_DESCRIPTOR_HANDLE ParticleManager::gpuDescHandleSRV;
D3D12_VERTEX_BUFFER_VIEW ParticleManager::vbView{};
//D3D12_INDEX_BUFFER_VIEW ParticleManager::ibView{};
ParticleManager::VertexPos ParticleManager::vertices[VERTEX_COUNT];
//unsigned short ParticleManager::indices[indexCount];
UINT ParticleManager::loadedTextureCount = 0;
Camera* ParticleManager::camera = nullptr;
std::vector<ParticleManager*> ParticleManager::drawList;
std::vector<ParticleManager*> ParticleManager::drawListFG;
std::unordered_map<std::wstring, UINT> ParticleManager::loadedTextureList;

void ParticleManager::StaticInitialize()
{

	ParticleManager::device = DX12Util::GetDevice();
	// コマンドリストをセット
	ParticleManager::cmdList = DX12Util::GetCmdList();
		
	// デスクリプタヒープの初期化
	CreateDescriptorHeap();

	// パイプライン初期化
	CreateGraphicsPipeline();

	// モデル生成
	CreateModel();

	loadedTextureList.clear();
}

void ParticleManager::BeginDraw()
{
	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate[0].Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ParticleManager::CreateDescriptorHeap()
{
	HRESULT result = S_FALSE;
	
	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = 512; // シェーダーリソースビュー1つ
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//生成
	if (FAILED(result)) {
		assert(0);
	}

	// デスクリプタサイズを取得
	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

void ParticleManager::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob;	// ジオメトリシェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/ParticleVS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
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

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/ParticlePS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
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

	// ジオメトリシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/ParticleGS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
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

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//スケール
			"TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0,
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
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//デプスの書き込みを禁止
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;

	//半透明合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//加算合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	//減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;
	//blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//
	gpipeline.BlendState.AlphaToCoverageEnable = true;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	if (FAILED(result)) {
		assert(0);
	}

	gpipeline.pRootSignature = rootsignature.Get();

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
		case PARTICLE_BLENDMODE_NORMAL:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case PARTICLE_BLENDMODE_ADD:
			blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case PARTICLE_BLENDMODE_SUB:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		case PARTICLE_BLENDMODE_MUL:
			blenddesc.SrcBlend = D3D12_BLEND_ZERO;
			blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		case PARTICLE_BLENDMODE_SCREEN:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_ONE;
			break;
		case PARTICLE_BLENDMODE_REVERSE:
			blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
			blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
			break;
		}

		////透明部分の深度値書き込み禁止
		//gpipeline.BlendState.AlphaToCoverageEnable = true;

		// ブレンドステートに設定する
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		//パイプラインステートの生成
		result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate[i].ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(result));
	}
}

void ParticleManager::LoadTexture(const std::wstring& filename)
{
	HRESULT result = S_FALSE;

	if (loadedTextureList.find(filename) != loadedTextureList.end())
	{
		texNumber = loadedTextureList.find(filename)->second;
		return;
	}

	// WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename.c_str(), WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) {
		assert(0);
	}

	const Image* img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	// テクスチャ用バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(texbuff[loadedTextureCount].ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
	}

	// テクスチャバッファにデータ転送
	result = texbuff[loadedTextureCount]->WriteToSubresource(
		0,
		nullptr, // 全領域へコピー
		img->pixels,    // 元データアドレス
		(UINT)img->rowPitch,  // 1ラインサイズ
		(UINT)img->slicePitch // 1枚サイズ
	);
	if (FAILED(result)) {
		assert(0);
	}

	// シェーダリソースビュー作成
	cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), loadedTextureCount, descriptorHandleIncrementSize);
	gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), loadedTextureCount, descriptorHandleIncrementSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff[loadedTextureCount]->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texbuff[loadedTextureCount].Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		cpuDescHandleSRV
	);

	loadedTextureList.emplace(filename, loadedTextureCount);

	loadedTextureCount++;

	texNumber = loadedTextureCount - 1;
}

void ParticleManager::CreateModel()
{
	HRESULT result = S_FALSE;

	std::vector<VertexPos> realVertices;
	
	////四角形の頂点データ
	//VertexPos verticesPoint[] = {
	//	//{{0.0f,0.0f,0.0f},{0,0,1},{0,1}},
	//	{{0.0f,0.0f,0.0f}}
	//};
	////メンバ変数にコピー
	//std::copy(std::begin(verticesPoint), std::end(verticesPoint), vertices);

	//for (int i = 0; i < vertexCount; i++)
	//{
	//	//X, Y, Z全て[-5.0f,+5.0f]でランダムに分布
	//	const float md_width = 10.0f;
	//	vertices[i].pos.x = (float)rand() / RAND_MAX * md_width - md_width / 2.0f;
	//	vertices[i].pos.y = (float)rand() / RAND_MAX * md_width - md_width / 2.0f;
	//	vertices[i].pos.z = (float)rand() / RAND_MAX * md_width - md_width / 2.0f;

	//}

	////四角形のインデックスデータ
	//unsigned short indicesSquare[] = {
	//	0,1,2,	//三角形1
	//	2,1,3,	//三角形2
	//};
	////メンバ変数にコピー
	//std::copy(std::begin(indicesSquare), std::end(indicesSquare), indices);

	// 頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	//// インデックスバッファ生成
	//result = device->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices)),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&indexBuff));
	//if (FAILED(result)) {
	//	assert(0);
	//	return;
	//}

	// 頂点バッファへのデータ転送
	VertexPos* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//// インデックスバッファへのデータ転送
	//unsigned short* indexMap = nullptr;
	//result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//if (SUCCEEDED(result)) {

	//	// 全インデックスに対して
	//	for (int i = 0; i < _countof(indices); i++)
	//	{
	//		indexMap[i] = indices[i];	// インデックスをコピー
	//	}

	//	indexBuff->Unmap(0, nullptr);
	//}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//// インデックスバッファビューの作成
	//ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//ibView.Format = DXGI_FORMAT_R16_UINT;
	//ibView.SizeInBytes = sizeof(indices);
}

void ParticleManager::DrawAll()
{
	//オブジェクト描画
	BeginDraw();
	for (auto& v : drawList) {
		v->DrawPrivate();
	}
	drawList.clear();
}

void ParticleManager::DrawAllFG()
{
	//深度バッファクリア
	DX12Util::ClearDepthBuffer(false);
	//オブジェクト描画
	BeginDraw();
	for (auto& v : drawListFG) {
		v->DrawPrivate();
	}
	drawListFG.clear();
}

bool ParticleManager::Initialize()
{
	// nullptrチェック
	assert(device);

	HRESULT result;
	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	return true;
}

void ParticleManager::Update()
{
	HRESULT result;
	//XMMATRIX matScale, matRot, matTrans;

	//// スケール、回転、平行移動行列の計算
	//matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//matRot = XMMatrixIdentity();
	//matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	//matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	//matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	//matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//// ワールド行列の合成
	//matWorld = XMMatrixIdentity(); // 変形をリセット

	//if(billboardType == 1) 	matWorld *= matBillboard;	//ビルボード行列を掛ける
	//else if (billboardType == 2) matWorld *= matBillboardY;	//Y軸回りビルボード行列を掛ける

	//matWorld *= matScale; // ワールド行列にスケーリングを反映
	//matWorld *= matRot; // ワールド行列に回転を反映
	//matWorld *= matTrans; // ワールド行列に平行移動を反映

	//// 親オブジェクトがあれば
	//if (parent != nullptr) {
	//	// 親オブジェクトのワールド行列を掛ける
	//	matWorld *= parent->matWorld;
	//}

	//寿命が尽きたパーティクルを全削除
	particles.remove_if(
		[](Particle& x) {
		return x.life.GetIsEnd();
	}
	);

	//全パーティクル更新
	for (std::forward_list<Particle>::iterator it = particles.begin();
		it != particles.end(); it++)
	{
		//時間更新
		it->life.Update();
		//速度に加速度を加算
		it->velocity = it->velocity + it->accel;
		//速度による移動
		it->position = it->position + it->velocity;

		//スケール更新
		//進行度を0～1の範囲に換算
		float f = (float)it->life.GetNowTime() / it->life.GetEndTime();

		//スケール
		if (f < 0.5f) {
			it->scale = it->s_scale + (it->m_scale - it->s_scale) * f * 2.0f;
		}
		else {
			it->scale = it->m_scale + (it->e_scale - it->m_scale) * (f - 0.5f) * 2.0f;
		}
	}

	//頂点バッファへデータ転送
	VertexPos* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		//パーティクルの情報を1つずつ反映
		for (std::forward_list<Particle>::iterator it = particles.begin();
			it != particles.end(); it++)
		{
			//座標
			vertMap->pos = it->position;
			//スケール
			vertMap->scale = it->scale;
			//次の頂点へ
			vertMap++;
		}
		vertBuff->Unmap(0, nullptr);
	}

	XMMATRIX matBillboard = XMMatrixIdentity();
	if (billboardType == BILLBOARD_TYPE_XYZ) {
		matBillboard = camera->GetBillboard();
	}
	else if (billboardType == BILLBOARD_TYPE_Y) {
		matBillboard = camera->GetBillboardY();
	}

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	//constMap->color = color;
	constMap->mat = camera->GetViewMatrix() * camera->GetProjectionMatrix();	//行列の合成
	constMap->matBillboard = matBillboard;	//ビルボードを転送
	constBuff->Unmap(0, nullptr);
}

void ParticleManager::Draw()
{
	drawList.emplace_back(this);
}

void ParticleManager::DrawFG()
{
	drawListFG.emplace_back(this);
}

void ParticleManager::Add(double life, const Vector3& position, const Vector3& velocity, const Vector3& accel, float start_scale, float end_scale)
{
	//リストに要素を追加
	particles.emplace_front();
	//追加した要素の参照
	Particle& p = particles.front();
	//値のセット
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.scale = start_scale;
	p.s_scale = start_scale;
	p.e_scale = end_scale;
	p.m_scale = start_scale + (end_scale - start_scale) * 0.5f;
	p.life.SetTimer(0, life);
	p.life.Start();
}

void ParticleManager::Add(double life, const Vector3& position, const Vector3& velocity, const Vector3& accel, float start_scale, float end_scale, float middle_scale)
{
	//リストに要素を追加
	particles.emplace_front();
	//追加した要素の参照
	Particle& p = particles.front();
	//値のセット
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.scale = start_scale;
	p.s_scale = start_scale;
	p.e_scale = end_scale;
	p.m_scale = middle_scale;
	p.life.SetTimer(0, life);
	p.life.Start();
}

void ParticleManager::DrawPrivate()
{
	// nullptrチェック
	assert(device);
	assert(ParticleManager::cmdList);

	//前回の描画に使用されたパイプラインのインデックスと今回使うものが違うなら
	//ここで切り替え
	static int prevPipelineIndex = 0;
	if (pipelineIndex != prevPipelineIndex) {
		//パイプラインステートの設定
		DX12Util::GetCmdList()->SetPipelineState(pipelinestate[pipelineIndex].Get());
	}

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	//cmdList->IASetIndexBuffer(&ibView);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);
	// 描画コマンド
	//cmdList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
	cmdList->DrawInstanced((UINT)std::distance(particles.begin(), particles.end()), 1, 0, 0);
}
