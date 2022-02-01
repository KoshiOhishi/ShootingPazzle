#pragma once
#include <vector>
#include "BaseBlock.h"
#include "BaseFloor.h"
#include "GameUtility.h"

class EditorStage
{
//�G�f�B�b�g�p�Ȃ̂Ō��J��OK
public:
	const float sphereRadius = ONE_CELL_LENGTH / 2;

	StageVec2 stageSize = { 20,20 };
	std::vector<BaseBlock*> blocks;
	std::vector<BaseFloor*> floors;
	Plane floorCollision;
	unsigned short startLaneZ = stageSize.y - 2;

	InstancingObjectDraw iodSquareBlock[4];
	InstancingObjectDraw iodTriangleBlock[4];
	InstancingObjectDraw iodNormalFloor;
	InstancingObjectDraw iodSwitchFloor[2];
	InstancingObjectDraw iodTurnFloor[8];
	InstancingObjectDraw iodBreakFloor;

public:
	EditorStage(){}
	~EditorStage();

	void Initialize();

	/// <summary>
	/// �X�e�[�W��ǂݍ���ŏ����� �t�@�C����������Ȃ��ꍇ��20x20�̃u���b�N�Ȃ��𐶐�
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
	/// ���ɏ��u���b�N���z�u����Ă��邩�`�F�b�N
	/// </summary>
	/// <param name="stagePos">�X�e�[�W��̍��W</param>
	/// <returns>���݂���΃u���b�N�z��̃C���f�b�N�X�A�Ȃ����-1��Ԃ�</returns>
	int CheckExistFloor(const StageVec2& stagePos);

	const Plane& GetFloorCollision()const { return floorCollision; }
};

