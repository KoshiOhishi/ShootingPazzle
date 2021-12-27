#pragma once
#include <vector>
#include "BaseBlock.h"
#include "BaseFloor.h"
#include "Timer.h"

class Stage
{
	friend class BaseBlock;

private:
	//�����a
	const float sphereRadius = ONE_CELL_LENGTH / 2;
	//�X�e�[�W�̑傫�� �}�X�P��
	StageVec2 stageSize = { 20,20 };
	//�u���b�N�R���e�i
	std::vector<BaseBlock*> blocks;
	//���u���b�N�R���e�i
	std::vector<BaseFloor*> floors;
	//�}�E�X�J�[�\���Ɣ�������p�̔R���W����
	Plane floorCollision;
	//���̃X�^�[�g���s
	unsigned short startLaneZ = stageSize.y - 2;
	//�c��ڕW�j��u���b�N��
	int targetBlockCount = 0;
	//�o���G�t�F�N�g�^�C�}�[
	Timer effectTimer;

public:
	Stage() {}
	~Stage();

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
	/// �`��I���֐�
	/// </summary>
	void EndDraw();

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
	/// �G�t�F�N�g�^�C�}�[���X�^�[�g������
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="speed"></param>
	void StartEffectTimer(int start, int end, float speed = 1.0);

	/// <summary>
	/// �u���b�N�z�u�擾
	/// </summary>
	/// <returns></returns>
	std::vector<BaseBlock*>& GetBlocks() { return blocks; }

	/// <summary>
	/// ���u���b�N�z�u�擾
	/// </summary>
	/// <returns></returns>
	std::vector<BaseFloor*>& GetFloors() { return floors; }

	/// <summary>
	/// �}�E�X�Ə��̓����蔻��Ɏg�p����R���W�����擾
	/// </summary>
	/// <returns></returns>
	const Plane& GetFloorCollision()const { return floorCollision; }

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
};