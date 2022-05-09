#include "SphereCollider.h"

using namespace DX12Library;

void SphereCollider::Update()
{
	//ワールド行列から座標を抽出
	const DirectX::XMMATRIX& matWorld = object3D->GetMatWorld();

	//球のメンバ変数を更新
	Vector3 mat = {
		matWorld.r[3].m128_f32[0],
		matWorld.r[3].m128_f32[1],
		matWorld.r[3].m128_f32[2],
	};

	Sphere::center = mat + offset;
	Sphere::radius = radius;
}
