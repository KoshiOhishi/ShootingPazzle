#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector2.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"

class Block
{
protected:
	std::vector<Capsule> capsule;
	Object3D object;

public:

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~Block() {};

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize(float posX, float posZ, float sphereRadius) = 0;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	virtual void UpdateCollision() = 0;

	/// <summary>
	/// �u���b�N�̎�ނ��擾����
	/// </summary>
	/// <returns>�u���b�N�̎��</returns>
	virtual const std::string& GetObjectType() = 0;

#pragma region Setter
	/// <summary>
	/// ���W�Z�b�g (Y���W�Œ�)
	/// </summary>
	/// <param name="pos"></param>
	void SetBlockPos(float x, float z);
#pragma endregion
#pragma region Getter

	const Vector3& GetPosition() { return object.GetPosition(); };

	/// <summary>
	/// �J�v�Z�������蔻����擾
	/// </summary>
	/// <returns>�J�v�Z�������蔻��</returns>
	const std::vector<Capsule> GetCapsule()const {
		return capsule;
	}

#pragma endregion
};

