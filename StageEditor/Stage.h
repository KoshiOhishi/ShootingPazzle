#pragma once
#include <vector>
#include "Block.h"
#include "Floor.h"

/// <summary>
/// �X�e�[�W���W ���オ���_
/// </summary>
struct StagePos {
	int x = 0;
	int y = 0;
};

/// <summary>
/// �u���b�N�^�C�v
/// </summary>
enum BlockType {
	BLOCKTYPE_SQUARE,
	BLOCKTYPE_TRIANGLE
};

class Stage
{
//�G�f�B�b�g�p�Ȃ̂Ō��J��OK
public:
	int width = 20;
	int depth = 20;
	std::vector<Block*> blocks;
	Floor floor;

public:
	Stage(){}
	~Stage();

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
	/// <param name="shapeType">�I�v�V�����̌`��w��</param>
	void AddBlock(const StagePos& stagePos, int blockType, int shapeType = 0);

	/// <summary>
	/// �u���b�N���폜 (�����̈ʒu�ɂȂɂ��Ȃ��ꍇ�͍폜���Ȃ�)
	/// </summary>
	/// <param name="stagePos"></param>
	void DeleteBlock(const StagePos& stagePos);

	/// <summary>
	/// ���Ƀu���b�N���z�u����Ă��邩�`�F�b�N
	/// </summary>
	/// <param name="stagePos">�X�e�[�W��̍��W</param>
	/// <returns>���݂���΃u���b�N�z��̃C���f�b�N�X�A�Ȃ����-1��Ԃ�</returns>
	int CheckExistBlock(float worldX, float worldZ);
};

