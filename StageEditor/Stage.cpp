#include <fstream>

#include "Stage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "GameUtility.h"

Stage::~Stage()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();
}

void Stage::LoadStage(std::string filename)
{
	//�f�[�^������
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();


	std::ifstream file;
	//�o�C�i�����[�h�ŊJ��
	file.open(filename, std::ios_base::binary);
	//�J���Ȃ�������u���b�N�Ȃ��f�[�^�𐶐�
	if (!file.is_open()) {
		stageSize.x = 20;
		stageSize.y = 20;
		startLaneZ = stageSize.y - 2;
		floor.CreateModel(stageSize.x * ONE_CELL_LENGTH, stageSize.y * ONE_CELL_LENGTH);
		floor.Initialize();
		return;
	}

	//�X�e�[�W���i�[
	StageHeader header;
	file.read((char*)&header, sizeof(header));
	stageSize.x = header.width;
	stageSize.y = header.depth;
	startLaneZ = header.startLineZ;

	//�u���b�N���i�[
	for (int i = 0; i < header.objectCount; i++) {
		StageObject object;
		file.read((char*)&object, sizeof(object));

		float x, z;
		StageVec2 pos = { object.stagePosX, object.stagePosY };
		GameUtility::CalcStagePos2WorldPos(pos, stageSize, &x, &z);

		//SquareBlock
		if (object.type == 0) {
			SquareBlock* newBlock = new SquareBlock;
			newBlock->Initialize(x, z, ONE_CELL_LENGTH / 2);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LT
		else if(object.type == 1) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(x, z, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_LEFTTOP);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RT
		else if (object.type == 2) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(x, z, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_RIGHTTOP);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LB
		else if (object.type == 3) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(x, z, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_LEFTBOTTOM);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RB
		else if (object.type == 4) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(x, z, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_RIGHTBOTTOM);
			blocks.emplace_back(newBlock);
		}
	}

	file.close();

	floor.CreateModel(stageSize.x * ONE_CELL_LENGTH, stageSize.y * ONE_CELL_LENGTH);
	floor.Initialize();
}

void Stage::Update()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Update();
	}
	floor.Update();
}

void Stage::Draw()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Draw();
	}
	floor.Draw();
}

void Stage::AddBlock(const StageVec2& stagePos, int blockType, int shapeType)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(stagePos, stageSize, &x, &z);

	//���Ƀu���b�N���z�u����Ă����烊�^�[��
	if (CheckExistBlock(x, z) != -1) {
		return;
	}

	//��ނ��ƂɃu���b�N�ǉ�
	if (blockType == BLOCKTYPE_SQUARE) {
		SquareBlock* newBlock = new SquareBlock;
		newBlock->Initialize(x, z, ONE_CELL_LENGTH / 2);

		blocks.emplace_back(newBlock);
	}
	else if (blockType == BLOCKTYPE_TRIANGLE) {
		TriangleBlock* newBlock = new TriangleBlock;
		newBlock->Initialize(x, z, ONE_CELL_LENGTH / 2);
		newBlock->SetShapeType(shapeType);

		blocks.emplace_back(newBlock);
	}
}

void Stage::DeleteBlock(const StageVec2& stagePos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(stagePos, stageSize, &x, &z);

	//�u���b�N���z�u����Ă��Ȃ������烊�^�[��
	int deleteIndex = CheckExistBlock(x, z);
	if (deleteIndex == -1) {
		return;
	}

	//�u���b�N�폜
	blocks.erase(blocks.begin() + deleteIndex);
}

int Stage::CheckExistBlock(float worldX, float worldZ)
{
	for (int i = 0; i < blocks.size(); i++) {
		if (worldX == blocks[i]->GetPosition().x && worldZ == blocks[i]->GetPosition().z) {
			return i;
		}
	}

	return -1;
}
