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
	//���_�o�b�t�@�̐���
	//���_�f�[�^�S�̂̃T�C�Y
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUVSkin) * vertices.size());
	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf())
	);

	//���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUVSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[(VBV)�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//�C���f�b�N�X�o�b�t�@�̐���
	//���_�C���f�b�N�X�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indexBuff.ReleaseAndGetAddressOf())
	);

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	//�C���f�b�N�X�o�b�t�@�r���[(IBV)�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;



	//�e�N�X�`���摜�f�[�^
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);	//���f�[�^���o
	assert(img);

	//���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//�e�N�X�`���p�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	//�e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(texBuff.ReleaseAndGetAddressOf())
	);

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuff->WriteToSubresource(
		0,
		nullptr,	//�S�̈�փR�s�[
		img->pixels,	//���f�[�^�A�h���X
		(UINT)img->rowPitch,	//1���C���T�C�Y
		(UINT)img->slicePitch	//1���T�C�Y
	);

	//�V�F�[�_���\�[�X�r���[(SRV)����
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texBuff.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
			&srvDesc, //�e�N�X�`���ݒ���
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			Object3D::GetDescHeap()->GetCPUDescriptorHandleForHeapStart(),
			Object3D::GetLoadCount(),
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		));

	//�萔�o�b�t�@�̍쐬
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuffB1.ReleaseAndGetAddressOf()));

	assert(SUCCEEDED(result));


	//�萔�o�b�t�@�̍쐬
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
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

	// �萔�o�b�t�@�փf�[�^�]��
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

	// �萔�o�b�t�@�փf�[�^�]��
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

	//���_�o�b�t�@���Z�b�g(VBV)
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//�C���f�b�N�X�o�b�t�@���Z�b�g(IBV)
	cmdList->IASetIndexBuffer(&ibView);

	if (isShadow == false) {
		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(2, constBuffB1->GetGPUVirtualAddress());

		//�萔�o�b�t�@�r���[���Z�b�g
		DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(5, constBuffMaterial->GetGPUVirtualAddress());

		//�V�F�[�_�[���\�[�X�r���[���Z�b�g
		cmdList->SetGraphicsRootDescriptorTable(6, CD3DX12_GPU_DESCRIPTOR_HANDLE(
			Object3D::GetDescHeap()->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			DX12Util::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		));
	}

	//�`��R�}���h
	//2�Ԗڂ̈�����ς���(�C���X�^���V���O)
	cmdList->DrawIndexedInstanced((UINT)indices.size(), instancingCount, 0, 0, 0);
}

void FbxModel::CreateFromFBX(const string& modelName)
{
	FbxLoader::GetInstance()->LoadModelFromFile(this, modelName);
}
