#include "MeshCollider.h"
#include "Collision.h"
#include "Matrix4.h"

using namespace DirectX;
using namespace DX12Library;

void MeshCollider::ConstructTriangles(ObjModel* model)
{
	triangles.clear();

	//const std::vector<Mesh*>& meshes = model->GetMeshes();

	int start = 0;

	//std::vector<Mesh*>::const_iterator it = meshes.cbegin();
	//for (; it != meshes.cend(); ++it) 
	{
		
		const std::vector<ObjModel::Vertex>& vertices = model->GetVertices();
		const std::vector<unsigned short>& indices = model->GetIndices();

		size_t triangleNum = indices.size() / 3;

		triangles.resize(triangles.size() + triangleNum);

		for (int i = 0; i < triangleNum; i++) {

			Triangle& tri = triangles[start + i];
			int idx0 = indices[i * 3 + 0];
			int idx1 = indices[i * 3 + 1];
			int idx2 = indices[i * 3 + 2];

			tri.p0 = {
				vertices[idx0].pos.x,
				vertices[idx0].pos.y,
				vertices[idx0].pos.z
			};

			tri.p1 = {
				vertices[idx1].pos.x,
				vertices[idx1].pos.y,
				vertices[idx1].pos.z
			};

			tri.p2 = {
				vertices[idx2].pos.x,
				vertices[idx2].pos.y,
				vertices[idx2].pos.z
			};

			tri.ComputeNormal();
		}
	}
}

void MeshCollider::Update()
{
	invMatWorld = XMMatrixInverse(nullptr, GetObject3d()->GetMatWorld());
	pNowCollision = nullptr;
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere, Vector3* inter)
{
	// オブジェクトのローカル座標系での球を得る（半径はXスケールを参照)
	Sphere localSphere;
	localSphere.center = Matrix4Transform(sphere.center, invMatWorld);
	localSphere.radius *= XMVector3Length(invMatWorld.r[0]).m128_f32[0];

	std::vector<Triangle>::const_iterator it = triangles.cbegin();

	for (; it != triangles.cend(); ++it) {
		const Triangle& triangle = *it;

		if (Collision::CheckSphere2Triangle(localSphere, triangle, inter)) {
			if (inter) {
				const XMMATRIX& matWorld = GetObject3d()->GetMatWorld();

				*inter = Matrix4Transform(*inter, matWorld);
			}
			pNowCollision = &triangle;
			return true;
		}
	}

	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, Vector3* inter)
{
	// オブジェクトのローカル座標系でのレイを得る
	Ray localRay;
	localRay.start = Matrix4Transform(ray.start, invMatWorld);
	XMMATRIX m = invMatWorld;
	m.r[3] = { 0,0,0,1 };
	localRay.dir = Matrix4Transform(ray.dir, m);

	std::vector<Triangle>::const_iterator it = triangles.cbegin();

	float shortestDistance = D3D12_FLOAT32_MAX;
	Vector3 tempInter;
	bool result = false;

	for (; it != triangles.cend(); ++it) {
		const Triangle& triangle = *it;

		float tmp;

		if (Collision::CheckRay2Triangle(localRay, triangle, &tmp, &tempInter)) {

			if (tmp > shortestDistance) {
				continue;
			}
			shortestDistance = tmp;

			const XMMATRIX& matWorld = GetObject3d()->GetMatWorld();

			tempInter = Matrix4Transform(tempInter, matWorld);

			pNowCollision = &triangle;
			result = true;
		}
	}

	if (result) {
		if (distance) {
			Vector3 sub = tempInter - ray.start;
			*distance = sub.Dot(ray.dir);
		}

		if (inter) {
			*inter = tempInter;
		}
	}

	return result;
}

const Triangle MeshCollider::GetNowCollisionTriangleInfo()
{
	//ワールド座標系に変換して出力
	Triangle t = *pNowCollision;
	XMMATRIX m = GetObject3d()->GetMatWorld();
	t.p0 = Matrix4Transform(t.p0, m);
	t.p1 = Matrix4Transform(t.p1, m);
	t.p2 = Matrix4Transform(t.p2, m);
	m.r[3] = { 0,0,0,1 };
	t.normal = Matrix4Transform(t.normal, m);

	return t;
}