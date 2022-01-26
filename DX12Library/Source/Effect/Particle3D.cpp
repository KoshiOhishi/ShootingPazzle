#include "Particle3D.h"
#include "DX12Util.h"
#include "DrawManager.h"

Camera* Particle3D::camera;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Particle3D::rootsignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Particle3D::pipelinestate[PIPELINE_COUNT];

using namespace DirectX;

void Particle3D::StaticInitialize()
{
	// パイプライン初期化
	CreateGraphicsPipeline();
}

void Particle3D::BeginDraw()
{
	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();
	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate[0].Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void Particle3D::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob;	// ジオメトリシェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	ID3D12Device* device = DX12Util::GetDevice();

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/Particle3DVS.hlsl",	// シェーダファイル名
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
		L"Shader/Particle3DPS.hlsl",	// シェーダファイル名
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
		L"Shader/Particle3DGS.hlsl",	// シェーダファイル名
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

	//デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	//標準値をセット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

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

	//透明部分の深度値書き込み設定
	gpipeline.BlendState.AlphaToCoverageEnable = false;

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

		//透明部分の深度値書き込み設定
		gpipeline.BlendState.AlphaToCoverageEnable = false;

		// ブレンドステートに設定する
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		//パイプラインステートの生成
		result = DX12Util::GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate[i].ReleaseAndGetAddressOf()));
		assert(SUCCEEDED(result));
	}
}

void Particle3D::CreateModel()
{
	HRESULT result = S_FALSE;
	ID3D12Device* device = DX12Util::GetDevice();

	std::vector<VertexPos3D> realVertices;

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

	// 頂点バッファへのデータ転送
	VertexPos3D* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);
}

void Particle3D::Initialize()
{
	HRESULT result;
	ID3D12Device* device = DX12Util::GetDevice();

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData3D) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	// モデル生成
	CreateModel();
}

void Particle3D::Update()
{
	HRESULT result;

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
	VertexPos3D* vertMap = nullptr;
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
	ConstBufferData3D* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->mat = camera->GetViewMatrix() * camera->GetProjectionMatrix();	//行列の合成
		constMap->matBillboard = matBillboard;	//ビルボードを転送
		constMap->color = color;
		constBuff->Unmap(0, nullptr);
	}
}

void Particle3D::Draw()
{
	DrawManager::AddDrawList(DRAW_MANAGER_OBJECT_TYPE_PARTICLE3D, this);
}

void Particle3D::DrawPrivate()
{
	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();

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
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap.Get()->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			descriptorHandleIncrementSize)
	);
	// 描画コマンド
	//cmdList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
	cmdList->DrawInstanced((UINT)std::distance(particles.begin(), particles.end()), 1, 0, 0);
}

void Particle3D::Add(double life, const Vector3& position, const Vector3& velocity, const Vector3& accel, float start_scale, float end_scale)
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

void Particle3D::Add(double life, const Vector3& position, const Vector3& velocity, const Vector3& accel, float start_scale, float end_scale, float middle_scale)
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