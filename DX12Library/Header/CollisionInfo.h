#pragma once

#include "Vector3.h"

namespace DX12Library {

	class Object3D;
	class BaseCollider;

	/// <summary>
	/// 衝突情報
	/// </summary>
	struct CollisionInfo
	{
	public:
		CollisionInfo(Object3D* object, BaseCollider* collider, const Vector3& inter) {
			this->object = object;
			this->collider = collider;
			this->inter = inter;
		}

		// 衝突相手のオブジェクト
		Object3D* object = nullptr;
		// 衝突相手のコライダー
		BaseCollider* collider = nullptr;
		// 衝突点
		Vector3 inter;
	};

}