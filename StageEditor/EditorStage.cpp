#include <fstream>

#include "EditorStage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "GameUtility.h"

EditorStage::~EditorStage()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();
}

void EditorStage::LoadStage(std::string filename)
{
	//�f�[�^������
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();

	//GameUtility�ɃX�e�[�W�T�C�Y��n��
	GameUtility::SetPStageSize(&stageSize);

	std::ifstream file;
	//�o�C�i�����[�h�ŊJ��
	file.open(filename, std::ios_base::binary);
	//�J���Ȃ�������u���b�N�Ȃ��f�[�^�𐶐�
	if (!file.is_open()) {
		stageSize.x = 20;
		stageSize.y = 20;
		startLaneZ = stageSize.y - 2;
		floor.CreateModel(stageSize);
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

		StageVec2 pos = { object.stagePosX, object.stagePosY };

		//SquareBlock
		if (object.type == 0) {
			SquareBlock* newBlock = new SquareBlock;
			newBlock->Initialize(pos, ONE_CELL_LENGTH / 2);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LT
		else if(object.type == 1) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_LEFTTOP);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RT
		else if (object.type == 2) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_RIGHTTOP);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LB
		else if (object.type == 3) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_LEFTBOTTOM);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RB
		else if (object.type == 4) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, ONE_CELL_LENGTH / 2);
			newBlock->SetShapeType(SHAPETYPE_NO_RIGHTBOTTOM);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
	}

	file.close();

	floor.CreateModel(stageSize);
	floor.Initialize();
}

void EditorStage::Update()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Update();
	}
	floor.Update();
}

void EditorStage::Draw()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Draw();
	}
	floor.Draw();
}

void EditorStage::AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount, int shapeType)
{
	//���Ƀu���b�N���z�u����Ă����烊�^�[��
	if (CheckExistBlock(stagePos) != -1) {
		return;
	}

	//��ނ��ƂɃu���b�N�ǉ�
	if (blockType == BLOCKTYPE_SQUARE) {
		SquareBlock* newBlock = new SquareBlock;
		newBlock->Initialize(stagePos, ONE_CELL_LENGTH / 2);
		newBlock->SetBreakupCount(breakupCount);
		blocks.emplace_back(newBlock);
	}
	else if (blockType == BLOCKTYPE_TRIANGLE) {
		TriangleBlock* newBlock = new TriangleBlock;
		newBlock->Initialize(stagePos, ONE_CELL_LENGTH / 2);
		newBlock->SetShapeType(shapeType);
		newBlock->SetBreakupCount(breakupCount);
		blocks.emplace_back(newBlock);
	}
}

void EditorStage::DeleteBlock(const StageVec2& stagePos)
{
	//�u���b�N���z�u����Ă��Ȃ������烊�^�[��
	int deleteIndex = CheckExistBlock(stagePos);
	if (deleteIndex == -1) {
		return;
	}

	//�u���b�N�폜
	if (blocks[deleteIndex]) delete blocks[deleteIndex];
	blocks.erase(blocks.begin() + deleteIndex);
}

int EditorStage::CheckExistBlock(const StageVec2& stagePos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(stagePos, &x, &z);

	for (int i = 0; i < blocks.size(); i++) {
		if (x == blocks[i]->GetPosition().x && z == blocks[i]->GetPosition().z) {
			return i;
		}
	}

	return -1;
}
