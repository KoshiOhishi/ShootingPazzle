#pragma once
#include "InstancingObject.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"

class BaseFloor
{
protected:
	InstancingObject object;

	//�o���G�t�F�N�g�̎��
	int firstEffectType = -1;
	//�o���G�t�F�N�g�̏I������
	double firstEffectEndTime = -1;
	//�N���A�G�t�F�N�g�̊J�n����
	double clearEffectStartTime = -1;

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
	/// �o���G�t�F�N�g�̍X�V
	/// </summary>
	virtual void UpdateFirstEffect(const Timer& timer);

	/// <summary>
	/// �N���A�G�t�F�N�g�̍X�V
	/// </summary>
	/// <param name="timer"></param>
	virtual void UpdateClearEffect(const Timer& timer);

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

