#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "Object3D.h"

namespace DX12Library {

	/// <summary>
	/// コライダー基底クラス
	/// </summary>
	class BaseCollider
	{
	public:
		BaseCollider() = default;
		virtual ~BaseCollider() = default;

		inline void SetObject(Object3D* object) {
			this->object3D = object;
		}

		inline Object3D* GetObject3d() {
			return object3D;
		}

		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update() = 0;

		inline CollisionShapeType GetShapeType() { return shapeType; }

		/// <summary>
		/// 衝突時コールバック関数
		/// </summary>
		/// <param name="info">衝突情報</param>
		inline void OnCollision(const CollisionInfo& info) {
			object3D->OnCollision(info);
		}

	protected:
		Object3D* object3D = nullptr;
		// 形状タイプ
		CollisionShapeType shapeType = SHAPE_UNKNOWN;
	};

}