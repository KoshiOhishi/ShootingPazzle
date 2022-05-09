#pragma once
#include <vector>
#include "BaseBlock.h"
#include "BaseFloor.h"
#include "Timer.h"
#include "Particle3D.h"
#include <memory>

class Stage
{
	friend class BaseBlock;
	friend class BaseFloor;

private:
	//�����a
	const float sphereRadius = ONE_CELL_LENGTH / 2;
	//�X�e�[�W�̑傫�� �}�X�P��
	StageVec2 stageSize = { 20,20 };
	//�u���b�N�R���e�i
	std::vector<std::unique_ptr<BaseBlock>> blocks;
	//���u���b�N�R���e�i
	std::vector< std::unique_ptr<BaseFloor>> floors;
	//�}�E�X�J�[�\���Ɣ�������p�̔R���W����
	DX12Library::Plane floorCollision;
	//���̃X�^�[�g���s
	unsigned short startLaneZ = stageSize.y - 2;
	//�c��ڕW�j��u���b�N��
	int targetBlockCount = 0;
	//�o���G�t�F�N�g�^�C�}�[�̃|�C���^
	DX12Library::Timer* pFirstEffectTimer;
	//�N���A�G�t�F�N�g�^�C�}�[�̃|�C���^
	DX12Library::Timer* pClearEffectTimer;

	DX12Library::InstancingObjectDraw iodSquareBlock[4];
	DX12Library::InstancingObjectDraw iodTriangleBlock[4];
	DX12Library::InstancingObjectDraw iodNormalFloor;
	DX12Library::InstancingObjectDraw iodSwitchFloor[2];
	DX12Library::InstancingObjectDraw iodTurnFloor[8];
	DX12Library::InstancingObjectDraw iodBreakFloor;

public:
	Stage();
	~Stage();

	void Initialize();

	/// <summary>
	/// �X�e�[�W��ǂݍ���ŏ�����
	/// </smmary>
	/// <param name="filename">�t�@�C���p�X</param>
	void LoadStage(std::string filename);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �o���G�t�F�N�g�̍X�V
	/// </summary>
	void UpdateFirstEffect();

	/// <summary>
	/// �N���A�G�t�F�N�g�X�V
	/// </summary>
	void UpdateClearEffect();

	/// <summary>
	/// ���u���b�N�󂫃}�X�Ɍ��I�u�W�F�N�g��}��
	/// </summary>
	void InsertHole();

	/// <summary>
	/// ���u���b�N�����ɔz�u����Ă��邩��Ԃ�
	/// </summary>
	/// <param name="stagePos">�X�e�[�W����W</param>
	/// <returns>���ɔz�u����Ă����floors�̃C���f�b�N�X�A�Ȃ����-1��Ԃ�</returns>
	int CheckExistFloor(const StageVec2& stagePos);

	/// <summary>
	/// �u���b�N��ǉ� (�����̈ʒu�Ɋ��Ƀu���b�N���z�u����Ă�����ǉ����Ȃ�)
	/// </summary>
	/// <param name="stagePos">�X�e�[�W��̍��W</param>
	/// <param name="blockType">�u���b�N�̎��</param>
	/// <param name="breakupCount">����܂ł̏Փˉ񐔁@0�ŉ��Ȃ��u���b�N</param>
	/// <param name="blockColor">�u���b�N�̐F�w��</param>
	void AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount, int blockColor);

	/// <summary>
	/// �u���b�N���폜 (�����̈ʒu�ɂȂɂ��Ȃ��ꍇ�͍폜���Ȃ�)
	/// </summary>
	/// <param name="stagePos"></param>
	void DeleteBlock(const StageVec2& stagePos);

	/// <summary>
	/// ���Ƀu���b�N���z�u����Ă��邩�`�F�b�N
	/// </summary>
	/// <param name="stagePos">�X�e�[�W��̍��W</param>
	/// <returns>���݂���΃u���b�N�z��̃C���f�b�N�X�A�Ȃ����-1��Ԃ�</returns>
	int CheckExistBlock(const StageVec2& stagePos);

	/// <summary>
	/// ���u���b�N��ǉ� (�����̈ʒu�Ɋ��Ƀu���b�N���z�u����Ă�����ǉ����Ȃ�)
	/// </summary>
	/// <param name="stagePos">�X�e�[�W��̍��W</param>
	/// <param name="floorType">���u���b�N�̎��</param>
	void AddFloor(const StageVec2& stagePos, int floorType);

	/// <summary>
	/// ���u���b�N���폜 (�����̈ʒu�ɂȂɂ��Ȃ��ꍇ�͍폜���Ȃ�)
	/// </summary>
	/// <param name="stagePos"></param>
	void DeleteFloor(const StageVec2& stagePos);

	/// <summary>
	/// �c��ڕW�j��u���b�N�������炷
	/// </summary>
	void DecrementTargetBlockCount() { targetBlockCount--; }

	/// <summary>
	/// �u���b�N�z�u�擾
	/// </summary>
	/// <returns></returns>
	std::vector<std::unique_ptr<BaseBlock>>& GetBlocks() { return blocks; }

	/// <summary>
	/// ���u���b�N�z�u�擾
	/// </summary>
	/// <returns></returns>
	std::vector<std::unique_ptr<BaseFloor>>& GetFloors() { return floors; }

	/// <summary>
	/// �}�E�X�Ə��̓����蔻��Ɏg�p����R���W�����擾
	/// </summary>
	/// <returns></returns>
	const DX12Library::Plane& GetFloorCollision()const { return floorCollision; }

	/// <summary>
	/// �X�^�[�g���[����z���W�擾
	/// </summary>
	/// <returns></returns>
	unsigned short GetStartLaneZ() { return startLaneZ; }

	/// <summary>
	/// �X�e�[�W�T�C�Y���擾 �P�ʂ̓}�X
	/// </summary>
	/// <returns></returns>
	const StageVec2& GetStageSize() { return stageSize; }

	/// <summary>
	/// �c��ڕW�j��u���b�N���擾
	/// </summary>
	/// <returns>�c��ڕW�j��u���b�N��</returns>
	const int GetTargetBlockCount() { return targetBlockCount; }

	/// <summary>
	/// �u���b�N�E���S�ĂɓK�p�����}�X�^�[���W
	/// </summary>
	/// <param name="pos"></param>
	void SetMasterPosition(const DX12Library::Vector3& pos);

	/// <summary>
	/// �e��`�悷�邩
	/// </summary>
	/// <param name="isDraw"></param>
	void SetDrawShadow(bool isDraw);

	void SetPFirstEffectTimer(DX12Library::Timer* pFirstEffectTimer) { this->pFirstEffectTimer = pFirstEffectTimer; }
	void SetPClearEffectTimer(DX12Library::Timer* pClearEffectTimer) { this->pClearEffectTimer = pClearEffectTimer; }
};