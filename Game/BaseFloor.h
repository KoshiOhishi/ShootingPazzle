#pragma once
#include "InstancingObject.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"
#include "Stage.h"
#include "Particle3D.h"

class Stage;

class BaseFloor
{
protected:
	//�C���X�^���V���O�`��I�u�W�F�N�g
	DX12Library::InstancingObject object;
	//�C���X�^���V���O�`��e�I�u�W�F�N�g�|�C���^
	DX12Library::InstancingObjectDraw* pIOD;

	//�X�e�[�W�|�C���^
	static Stage* pStage;
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
	virtual void UpdateFirstEffect(const DX12Library::Timer& timer);

	/// <summary>
	/// �N���A�G�t�F�N�g�̍X�V
	/// </summary>
	/// <param name="timer"></param>
	virtual void UpdateClearEffect(const DX12Library::Timer& timer);

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	virtual void UpdateCollision() = 0;

	/// <summary>
	/// ���u���b�N�̎�ނ�Ԃ�
	/// </summary>
	/// <returns></returns>
	virtual int GetFloorType() = 0;

	/// <summary>
	/// �X�e�[�W�|�C���^�Z�b�g
	/// </summary>
	/// <param name="pStage"></param>
	static void SetPStage(Stage* pStage) { BaseFloor::pStage = pStage; }

	/// <summary>
	/// ���W�Z�b�g (�X�e�[�W���W�)
	/// </summary>
	/// <param name="pos">�X�e�[�W��̍��W</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// ���W�Z�b�g
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const DX12Library::Vector3& pos) { object.SetPosition(pos); }

	/// <summary>
	/// �C���X�^���V���O�`��e�I�u�W�F�N�g�̃|�C���^�Z�b�g
	/// </summary>
	/// <param name="pIOD"></param>
	void SetPInstancingObjectDraw(DX12Library::InstancingObjectDraw* pIOD) { this->pIOD = pIOD; }

	const DX12Library::Vector3& GetPosition() { return object.GetPosition(); }
};

