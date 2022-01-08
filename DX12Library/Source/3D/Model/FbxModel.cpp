#include "FbxModel.h"
#include "DX12Util.h"
#include "Object3D.h"

FbxModel::~FbxModel()
{
	fbxScene->Destroy();
}


void FbxModel::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;
	//頂点バッファの生成
	//頂点データ全体のサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUVSkin) * vertices.size());
	//頂点バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf())
	);

	//頂点バッファへのデータ転送
	VertexPosNormalUVSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビュー(VBV)の作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//インデックスバッファの生成
	//頂点インデックス全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indexBuff.ReleaseAndGetAddressOf())
	);

	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	//インデックスバッファビュー(IBV)の作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;



	//テクスチャ画像データ
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);	//生データ抽出
	assert(img);

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//テクスチャ用バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(texBuff.ReleaseAndGetAddressOf())
	);

	//テクスチャバッファにデータ転送
	result = texBuff->WriteToSubresource(
		0,
		nullptr,	//全領域へコピー
		img->pixels,	//元データアドレス
		(UINT)img->rowPitch,	//1ラインサイズ
		(UINT)img->slicePitch	//1枚サイズ
	);

	//シェーダリソースビュー(SRV)生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texBuff.Get(), //ビューと関連付けるバッファ
			&srvDesc, //テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			Object3D::GetDescHeap()->GetCPUDescriptorHandleForHeapStart(),
			Object3D::GetLoadCount(),
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		));

	//定数バッファの作成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffB1.ReleaseAndGetAddressOf()));

	assert(SUCCEEDED(result));


	//定数バッファの作成
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffMaterial.ReleaseAndGetAddressOf()));

	assert(SUCCEEDED(result));

	TransferMaterial();

	texNumber = Object3D::GetLoadCount();
	Object3D::IncrementLoadCount();
}

void FbxModel::TransferMaterial()
{
	HRESULT result;

	// 定数バッファへデータ転送
	ConstBufferDataB1* constMapB1 = nullptr;
	result = constBuffB1->Map(0, nullptr, (void**)&constMapB1);
	if (SUCCEEDED(result)) {
		constMapB1->ambient = ambient;
		constMapB1->diffuse = diffuse;
		constMapB1->specular = specular;
		constMapB1->alpha = alpha;
		constBuffB1->Unmap(0, nullptr);
	}

	assert(SUCCEEDED(result));

	// 定数バッファへデータ転送
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	if (SUCCEEDED(result)) {
		constMapMaterial->baseColor = material.baseColor;
		constMapMaterial->metalness = material.metalness;
		constMapMaterial->specular = material.specular;
		constMapMaterial->roughness = material.roughness;
		constBuffMaterial->Unmap(0, nullptr);
	}

	assert(SUCCEEDED(result));
}

void FbxModel::Draw(int instancingCount, bool isShadow)
{
	ID3D12GraphicsCommandList* cmdList = DX12Util::GetCmdList();

	//頂点バッファをセット(VBV)
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//インデックスバッファをセット(IBV)
	cmdList->IASetIndexBuffer(&ibView);

	if (isShadow == false) {
		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffB1->GetGPUVirtualAddress());

		//定数バッファビューをセット
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(5, constBuffMaterial->GetGPUVirtualAddress());

		//シェーダーリソースビューをセット
		cmdList->SetGraphicsRootDescriptorTable(6, CD3DX12_GPU_DESCRIPTOR_HANDLE(
			Object3D::GetDescHeap()->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		));
	}

	//描画コマンド
	//2番目の引数を変える(インスタンシング)
	cmdList->DrawIndexedInstanced((UINT)indices.size(), instancingCount, 0, 0, 0);
}

void FbxModel::CreateFromFBX(const string& modelName)
{
	FbxLoader::GetInstance()->LoadModelFromFile(this, modelName);
}
