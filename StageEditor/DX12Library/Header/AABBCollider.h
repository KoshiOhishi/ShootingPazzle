#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include "Vector3.h"

/// <summary>
/// ���Փ˔���I�u�W�F�N�g
/// </summary>
class AABBCollider : public BaseCollider, public AABB
{
public:
	AABBCollider(Vector3 offset, Vector3 boxLength) :
		offset(offset),
		boxLength(boxLength)
	{
		// ���`����Z�b�g
		shapeType = COLLISIONSHAPE_AABB;
	}

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	//inline void SetMinPos(Vector3 minPos) { this->minPos = minPos; }
	//inline void SetMaxPos(Vector3 maxPos) { this->maxPos = maxPos; }

private:
	// �I�u�W�F�N�g���S����̃I�t�Z�b�g
	Vector3 offset;
	// BOX�̒���
	Vector3 boxLength;
};

