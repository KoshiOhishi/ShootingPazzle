#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

namespace DX12Library {

	/// <summary>
	/// ���b�V���Փ˔���I�u�W�F�N�g
	/// </summary>
	class MeshCollider :
		public BaseCollider
	{
	public:
		MeshCollider()
		{
			// ���b�V���`����Z�b�g
			shapeType = COLLISIONSHAPE_MESH;
		}

		/// <summary>
		/// �O�p�`�̔z����\�z����
		/// </summary>
		void ConstructTriangles(ObjModel* model);

		/// <summary>
		/// �X�V
		/// </summary>
		void Update() override;

		/// <summary>
		/// ���Ƃ̓����蔻��
		/// </summary>
		/// <param name="sphere">��</param>
		/// <param name="inter">��_�i�o�͗p�j</param>
		/// <returns>�������Ă��邩�ۂ�</returns>
		bool CheckCollisionSphere(const Sphere& sphere, Vector3* inter = nullptr);

		/// <summary>
		/// ���C�Ƃ̓����蔻��
		/// </summary>
		/// <param name="sphere">���C</param>
		/// <param name="distance">�����i�o�͗p�j</param>
		/// <param name="inter">��_�i�o�͗p�j</param>
		/// <returns>�������Ă��邩�ۂ�</returns>
		bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, Vector3* inter = nullptr);

		/// <summary>
		/// ���ݏՓ˂��Ă���O�p�`�̏����擾
		/// </summary>
		/// <returns></returns>
		const Triangle GetNowCollisionTriangleInfo();

	private:
		std::vector<Triangle> triangles;
		// ���[���h�s��̋t�s��
		DirectX::XMMATRIX invMatWorld;

		//�������Ă���O�p�`�̃|�C���^
		const Triangle* pNowCollision = nullptr;
	};

}