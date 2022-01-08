#include <d3dx12.h>
#include "DX12Util.h"
#include "Light.h"

using namespace DirectX;



void Light::Initialize()
{
	HRESULT result;
	//�萔�o�b�t�@�̐���
	result = DX12Util::GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) { assert(0); }

	//�萔�o�b�t�@�փf�[�^�]��
	TransferConstBuffer();
}

void Light::Update()
{
	//�l�̕ύX���������������萔�o�b�t�@�ɓ]������
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void Light::Draw(UINT rootParameterIndex)
{
	//�萔�o�b�t�@�r���[���Z�b�g
	DX12Util::GetCmdList()->SetGraphicsRootConstantBufferView(rootParameterIndex,constBuff->GetGPUVirtualAddress());
}

void Light::TransferConstBuffer()
{
	HRESULT result;
	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->lightDir = -lightDir;
		constMap->lightColor = lightColor;
		constBuff->Unmap(0, nullptr);
	}
}

void Light::CalcLightPos(float distance)
{
	//���C�g���W
	lightPos = lighttarget - lightDir * distance;
}

void Light::SetLightDir(const Vector3& lightDir)
{
	//���K�����ăZ�b�g
	this->lightDir = lightDir.Normalize();
	dirty = true;
}

void Light::SetLightDir(const float dirX, const float dirY, const float dirZ)
{
	Vector3 vec(dirX, dirY, dirZ);
	SetLightDir(vec);
}

void Light::SetLightColor(const Vector3& lightColor)
{
	this->lightColor = lightColor;
	dirty = true;
}

void Light::SetLightColor(const float R, const float G, const float B)
{
	SetLightColor({ R,G,B });
}

void Light::SetLightColorAs0To255(const float R, const float G, const float B)
{
	SetLightColor({ R / 255,G / 255,B / 255 });
}
