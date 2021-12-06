#pragma once
#include <vector>
#include "Block.h"
#include "Floor.h"

class Stage
{
private:
	int width;
	int depth;
	std::vector<Block*> blocks;
	Floor floor;

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
};

