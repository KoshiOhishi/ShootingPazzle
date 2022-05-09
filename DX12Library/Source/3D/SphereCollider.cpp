#include "SphereCollider.h"

using namespace DX12Library;

void SphereCollider::Update()
{
	//���[���h�s�񂩂���W�𒊏o
	const DirectX::XMMATRIX& matWorld = object3D->GetMatWorld();

	//���̃����o�ϐ����X�V
	Vector3 mat = {
		matWorld.r[3].m128_f32[0],
		matWorld.r[3].m128_f32[1],
		matWorld.r[3].m128_f32[2],
	};

	Sphere::center = mat + offset;
	Sphere::radius = radius;
}
