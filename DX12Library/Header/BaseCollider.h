#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "Object3D.h"

namespace DX12Library {

	/// <summary>
	/// �R���C�_�[���N���X
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
		/// �X�V
		/// </summary>
		virtual void Update() = 0;

		inline CollisionShapeType GetShapeType() { return shapeType; }

		/// <summary>
		/// �Փˎ��R�[���o�b�N�֐�
		/// </summary>
		/// <param name="info">�Փˏ��</param>
		inline void OnCollision(const CollisionInfo& info) {
			object3D->OnCollision(info);
		}

	protected:
		Object3D* object3D = nullptr;
		// �`��^�C�v
		CollisionShapeType shapeType = SHAPE_UNKNOWN;
	};

}