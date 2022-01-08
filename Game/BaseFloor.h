#pragma once
#include "InstancingObject.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"

class BaseFloor
{
protected:
	//�C���X�^���V���O�`��I�u�W�F�N�g
	InstancingObject object;
	//�C���X�^���V���O�`��e�I�u�W�F�N�g�|�C���^
	InstancingObjectDraw* pIOD;

	std::string objectName = "";

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
	/// ���W�Z�b�g (�X�e�[�W���W�)
	/// </summary>
	/// <param name="pos">�X�e�[�W��̍��W</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// ���W�Z�b�g
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos) { object.SetPosition(pos); }

	/// <summary>
	/// �C���X�^���V���O�`��e�I�u�W�F�N�g�̃|�C���^�Z�b�g
	/// </summary>
	/// <param name="pIOD"></param>
	void SetPInstancingObjectDraw(InstancingObjectDraw* pIOD) { this->pIOD = pIOD; }

	/// <summary>
	/// �u���b�N�̎�ނ��擾����
	/// </summary>
	/// <returns>�u���b�N�̎��</returns>
	const std::string& GetObjectName() { return objectName; }

	const Vector3& GetPosition() { return object.GetPosition(); }
};

