#include "CollisionManager.h"
#include "BaseCollider.h"
#include "Collision.h"
#include "MeshCollider.h"

using namespace DirectX;

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	//�S�Ă̑g�ݍ��킹�ɂ��đ�������`�F�b�N
	itA = colliders.begin();
	for (; itA != colliders.end(); ++itA) {
		itB = itA;
		++itB;
		for (; itB != colliders.end(); ++itB) {
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;

			//�Ƃ��ɋ�
			if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE) {
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Sphere* SphereB = dynamic_cast<Sphere*>(colB);
				Vector3 inter;
				if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter)) {
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			//���b�V���Ƌ�
			else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
				Sphere* sphere = dynamic_cast<Sphere*>(colB);
				Vector3 inter;
				if (meshCollider->CheckCollisionSphere(*sphere, &inter)) {
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_MESH) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
				Sphere* sphere = dynamic_cast<Sphere*>(colA);
				Vector3 inter;
				if (meshCollider->CheckCollisionSphere(*sphere, &inter)) {
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			//�Ƃ���AABB
			if (colA->GetShapeType() == COLLISIONSHAPE_AABB &&
				colB->GetShapeType() == COLLISIONSHAPE_AABB) {
				AABB* aabbA = dynamic_cast<AABB*>(colA);
				AABB* aabbB = dynamic_cast<AABB*>(colB);
				Vector3 inter;
				if (Collision::CheckAABB2AABB(*aabbA, *aabbB, &inter)) {
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			//����AABB
			else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_AABB) {
				Sphere* sphere = dynamic_cast<Sphere*>(colA);
				AABB* aabb = dynamic_cast<AABB*>(colB);
				//��_�擾������
				Vector3 inter = {};
				if (Collision::CheckAABB2Sphere(*aabb, *sphere)) {
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_AABB &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE) {
				Sphere* sphere = dynamic_cast<Sphere*>(colB);
				AABB* aabb = dynamic_cast<AABB*>(colA);
				//��_�擾������
				Vector3 inter = {};
				if (Collision::CheckAABB2Sphere(*aabb, *sphere)) {
					colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
		}
	}
}

bool CollisionManager::Raycast(const Ray& ray, RaycastHit* hitInfo, const BaseCollider* ignoreCollider, float maxDistance)
{
	bool result = false;
	std::forward_list<BaseCollider*>::iterator it;
	std::forward_list<BaseCollider*>::iterator it_hit;
	float distance = maxDistance;
	Vector3 inter;

	// �S�ẴR���C�_�[�Ƒ�����`�F�b�N
	it = colliders.begin();
	for (; it != colliders.end(); ++it) {
		BaseCollider* colA = *it;

		//��������R���C�_�[���w�肳��Ă���Ƃ�
		if (ignoreCollider && colA == ignoreCollider) {
			continue;
		}

		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphere = dynamic_cast<Sphere*>(colA);

			float tempDistance;
			Vector3 tempInter;

			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempInter)) continue;
			if (tempDistance >= distance) continue;

			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			Vector3 tempInter;
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &tempInter)) continue;
			if (tempDistance >= distance) continue;

			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
		else if (colA->GetShapeType() == COLLISIONSHAPE_AABB) {
			AABB* aabb = dynamic_cast<AABB*>(colA);

			float tempDistance;
			Vector3 tempInter;

			if (!Collision::CheckRay2AABB(ray, *aabb, &tempDistance, &tempInter)) continue;
			if (tempDistance >= distance) continue;

			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
	}

	if (result && hitInfo) {
		hitInfo->distance = distance;
		hitInfo->inter = inter;
		hitInfo->collider = *it_hit;
		hitInfo->object = hitInfo->collider->GetObject3d();
	}

	return result;
}
