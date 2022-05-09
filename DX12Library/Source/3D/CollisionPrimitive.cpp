#include "CollisionPrimitive.h"

using namespace DirectX;
using namespace DX12Library;

void Triangle::ComputeNormal()
{
	Vector3 p0_p1 = p1 - p0;
	Vector3 p0_p2 = p2 - p0;

	//外積により、2辺に垂直なベクトルを算出する
	normal = p0_p1.Cross(p0_p2);
	normal = normal.Normalize();
}
