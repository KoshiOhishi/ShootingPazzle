#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"

class BaseFloor
{
protected:
	Object3D object;

public:

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~BaseFloor() {};

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize(const StageVec2& pos) = 0;

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
	/// ���W�Z�b�g (Y���W�Œ�)
	/// </summary>
	/// <param name="pos">�X�e�[�W��̍��W</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// �u���b�N�̎�ނ��擾����
	/// </summary>
	/// <returns>�u���b�N�̎��</returns>
	virtual std::string GetObjectType() = 0;

	const Vector3& GetPosition() { return object.GetPosition(); }
};

