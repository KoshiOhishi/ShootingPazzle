#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include "Vector3.h"

/// <summary>
/// 球衝突判定オブジェクト
/// </summary>
class AABBCollider : public BaseCollider, public AABB
{
public:
	AABBCollider(Vector3 offset, Vector3 boxLength) :
		offset(offset),
		boxLength(boxLength)
	{
		// 球形状をセット
		shapeType = COLLISIONSHAPE_AABB;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	//inline void SetMinPos(Vector3 minPos) { this->minPos = minPos; }
	//inline void SetMaxPos(Vector3 maxPos) { this->maxPos = maxPos; }

private:
	// オブジェクト中心からのオフセット
	Vector3 offset;
	// BOXの長さ
	Vector3 boxLength;
};

