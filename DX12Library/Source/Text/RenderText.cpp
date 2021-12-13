#include "RenderText.h"
#include "DX12Util.h"


//ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> RenderText::rootSignature;
//パイプラインステート
Microsoft::WRL::ComPtr<ID3D12PipelineState> RenderText::pipelineState;
//射影行列
DirectX::XMMATRIX RenderText::matProjection;
//頂点シェーダオブジェクト
Microsoft::WRL::ComPtr<ID3DBlob> RenderText::vsBlob;
//ピクセルシェーダオブジェクト
Microsoft::WRL::ComPtr<ID3DBlob> RenderText::psBlob;
//エラーオブジェクト
Microsoft::WRL::ComPtr<ID3DBlob> RenderText::errorBlob;
//デスクリプタヒープ
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RenderText::descHeap;
//生成数
int RenderText::createdCount = 0;

void RenderText::StaticInitialize()
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	vsBlob = nullptr; // 頂点シェーダオブジェクト
	psBlob = nullptr; // ピクセルシェーダオブジェクト
	errorBlob = nullptr; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/TextVertexShader.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"VSmain", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
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
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Shader/TextPixelShader.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"PSmain", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
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
		exit(1);
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
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1.0f-ソースのアルファ値

	// ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//透明部分の深度値書き込み禁止
	gpipeline.BlendState.AlphaToCoverageEnable = true;

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
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0 レジスタ

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2];
	//定数用
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)
	//テクスチャ用
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	//サンプラーの設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT);

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

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 512;
	result = DX12Util::GetDevice()->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf())
	);
}

void RenderText::SetFontData(FontData fontData)
{
	fd = fontData;
	changeFD = true;
}

void RenderText::FirstInit(wstring str)
{
	HRESULT result = S_FALSE;

	//頂点バッファ生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf())
	);

	prevStr = str;
}

void RenderText::CreateFontTexture(FontData fontData, wstring str)
{
	HRESULT result = S_FALSE;

	if (number == -1) {
		number = createdCount;
		createdCount++;
	}

	//まずはフォント作成
	LOGFONT lf = {
		fontData.height, fontData.width, 0, 0,
		fontData.weight, fontData.italic, 0, 0,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		fontData.quality, DEFAULT_PITCH,
		L""
	};
	
	//フォント名格納
	for (int i = 0; i < 32; i++) {
		lf.lfFaceName[i] = fontData.fontName.data()[i];
	}

	HFONT hFont = CreateFontIndirectW(&lf);

	// 現在のウィンドウに適用
	// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	const int gradFlag = GGO_GRAY4_BITMAP;

	//階調の最大値
	int grad = 0;
	switch (gradFlag) {

	case GGO_GRAY2_BITMAP:
		grad = 4;
		break;

	case GGO_GRAY4_BITMAP:
		grad = 16;
		break;

	case GGO_GRAY8_BITMAP:
		grad = 64;
		break;
	}

	//フォントのビットマップ取得
	struct BitmapData {
		BYTE* pData;
		TEXTMETRIC tm;
		GLYPHMETRICS gm;
		DWORD size;
	};

	std::vector<BitmapData> bitmapDatas(str.size());
	//生成するテクスチャサイズ
	int texWidth = 0, texHeight = 0;

	for (int i = 0; i < str.size(); i++) {

		UINT code = (UINT)str.c_str()[i];

		TEXTMETRIC tm;
		GetTextMetricsW(hdc, &tm);
		GLYPHMETRICS gm;
		const MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };

		//サイズ取得
		DWORD size = GetGlyphOutlineW(hdc, code, gradFlag, &gm, 0, NULL, &mat);

		//ビットマップ取得
		BYTE* pData = new BYTE[size];
		GetGlyphOutlineW(hdc, code, gradFlag, &gm, size, pData, &mat);

		//リストに追加
		bitmapDatas[i].pData = pData;
		bitmapDatas[i].gm = gm;
		bitmapDatas[i].tm = tm;
		bitmapDatas[i].size = size;

		//フォントの幅と高さを元にテクスチャサイズを更新
		int fontWidth = (gm.gmBlackBoxX + 3) / 4 * 4;
		int fontHeight = tm.tmAscent + tm.tmDescent;
		
		if (fontWidth > gm.gmCellIncX) {
			texWidth += fontWidth;
		}
		else {
			texWidth += gm.gmCellIncX;
		}

		if (texHeight < fontHeight) {
			texHeight = fontHeight;
		}
	}

	//デバイスコンテキスト、フォントハンドル解放
	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);

	//フォントを書き込むためのテクスチャ作成
	//GPU側にテクスチャバッファを作成する
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		texWidth,
		(UINT)texHeight,
		1,
		1
	);

	//テクスチャバッファ生成
	result = DX12Util::GetDevice()->CreateCommittedResource(	//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(texbuff.ReleaseAndGetAddressOf())
	);

	//テクスチャ作成
	std::vector<TextureData> textureDatas(texWidth * texHeight);
	int offsetX = 0;

	for (int i = 0; i < bitmapDatas.size(); i++) {

		//空白のときはオフセットだけ加算して次へ
		if (bitmapDatas[i].gm.gmBlackBoxX <= 1 && bitmapDatas[i].gm.gmBlackBoxY <= 1) {
			offsetX += bitmapDatas[i].gm.gmCellIncX;
			continue;
		}

		//フォントの幅と高さ
		int fontWidth = (bitmapDatas[i].gm.gmBlackBoxX + 3) / 4 * 4;
		int fontHeight = bitmapDatas[i].gm.gmBlackBoxY;
		//高さオフセット
		int offsetY = bitmapDatas[i].tm.tmAscent - bitmapDatas[i].gm.gmptGlyphOrigin.y;

		// フォント情報の書き込み
		DWORD Alpha;
		for (int y = 0; y < fontHeight; y++)
		{
			for (int x = 0; x < fontWidth; x++)
			{
				Alpha = (255 * bitmapDatas[i].pData[y * fontWidth + x]) / grad;
				int index = (y + offsetY) * texWidth + offsetX + x + bitmapDatas[i].gm.gmptGlyphOrigin.x;

				if (textureDatas[index].A == 0) {
					textureDatas[index].A = Alpha * color.w;
				}
			}
		}

		offsetX += bitmapDatas[i].gm.gmCellIncX;
	}

	////テスト 四隅の点を黒くしてテクスチャサイズを明示的にする
	//textureDatas[0].R = textureDatas[0].G = textureDatas[0].B = 0;
	//textureDatas[texWidth - 1].R = textureDatas[texWidth - 1].G = textureDatas[texWidth - 1].B = 0;
	//textureDatas[texWidth * (texHeight - 1)].R = textureDatas[texWidth * (texHeight - 1)].G = textureDatas[texWidth * (texHeight - 1)].B = 0;
	//textureDatas[texWidth * texHeight - 1].R = textureDatas[texWidth * texHeight - 1].G = textureDatas[texWidth * texHeight - 1].B = 0;

	//textureDatas[0].A = 255;
	//textureDatas[texWidth - 1].A = 255;
	//textureDatas[texWidth * (texHeight - 1)].A = 255;
	//textureDatas[texWidth * texHeight - 1].A = 255;

	//テクスチャバッファにデータ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,	//全領域にコピー
		textureDatas.data(),	//元データアドレス
		sizeof(TextureData) * texWidth,	//1ラインサイズ
		sizeof(TextureData) * textureDatas.size()	//全サイズ
	);

	//ビットマップデータ解放
	for (int i = 0; i < bitmapDatas.size(); i++) {
		delete[] bitmapDatas[i].pData;
	}
	bitmapDatas.clear();
	textureDatas.clear();

	//実際にシェーダーリソースビューを生成する
	//シェーダーリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = texresDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	DX12Util::GetDevice()->CreateShaderResourceView(
		texbuff.Get(),	//ビューと関連付けるバッファ
		&srvDesc,	//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeap->GetCPUDescriptorHandleForHeapStart(),
			number,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);


	//頂点データ
	VertexPosUv vertices[] = {
		//	 x		y	  z		u	  v
		{{	0.0f,			(float)texHeight,	0.0f},	{0.0f,1.0f}},	//左下
		{{	0.0f,			0.0f,				0.0f},	{0.0f,0.0f}},	//左上
		{{(float)texWidth,	(float)texHeight,	0.0f},	{1.0f,1.0f}},	//右下
		{{(float)texWidth,  0.0f,				0.0f},	{1.0f,0.0f}},	//右上

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

	//定数バッファの生成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);

	//定数バッファにデータを転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;		//色指定 (RGBA)
	constMap->mat = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight(), 0.0f, 0.0f, 1.0f);	//平行投影行列の合成
	constBuff->Unmap(0, nullptr);

	//行列初期化
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, DX12Util::GetWindowWidth(), DX12Util::GetWindowHeight(), 0.0f, 0.0f, 1.0f
	);

	changeFD = false;
}

bool RenderText::IsReCreate(wstring str)
{
	bool flag = false;

	if (str != prevStr) {
		flag = true;
	}

	prevStr = str;

	return flag || changeFD;
}

void RenderText::DrawString(float x, float y, wstring str)
{
	//生成されていなかったらここで生成
	if (number == -1) {
		FirstInit(str);
		CreateFontTexture(fd, str);
	}
	//文字変更などで再生成が必要なら再生成
	else if (IsReCreate(str)) {
		CreateFontTexture(fd, str);
	}

	//パイプラインステートの設定
	DX12Util::GetCmdList()->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	DX12Util::GetCmdList()->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状を設定
	DX12Util::GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//座標代入
	position = DirectX::XMVectorSet(x, y, 0, 0);

	//ワールド行列の更新
	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation));
	matWorld *= DirectX::XMMatrixTranslationFromVector(position);

	//行列の転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->mat = matWorld * matProjection;		//行列の合成
	constBuff->Unmap(0, nullptr);


	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	DX12Util::GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//頂点バッファをセット
	DX12Util::GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファをセット
	DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());


	//シェーダリソースビューをセット
	DX12Util::GetCmdList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			number,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	//描画コマンド
	DX12Util::GetCmdList()->DrawInstanced(4, 1, 0, 0);

}

void RenderText::SetColor(float r, float g, float b, float a)
{
	color.x = r;
	color.y = g;
	color.z = b;
	color.w = a;
}

void RenderText::SetColorAs0To255(float r, float g, float b, float a)
{
	SetColor(r / DX12TEXT_COLOR_MAX_RATE, g / DX12TEXT_COLOR_MAX_RATE, b / DX12TEXT_COLOR_MAX_RATE, a / DX12TEXT_COLOR_MAX_RATE);
}
