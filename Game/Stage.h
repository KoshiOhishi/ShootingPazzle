#pragma once
#include <vector>
#include "Block.h"
#include "Floor.h"

/// <summary>
/// �X�e�[�W���W ���オ���_
/// </summary>
struct StageVec2 {
	unsigned short x = 0;
	unsigned short y = 0;
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
private:
	StageVec2 stageSize = { 20,20 };
	std::vector<Block*> blocks;
	Floor floor;
	unsigned short startLaneZ = stageSize.y - 2;

public:
	Stage(){}
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
	/// �u���b�N�z�u�擾
	/// </summary>
	/// <returns></returns>
	std::vector<Block*>& GetBlocks() { return blocks; }

	const Floor& GetFloor()const { return floor; }

	unsigned short GetStartLaneZ() { return startLaneZ; }

	const StageVec2& GetStageSize() { return stageSize; }
};

