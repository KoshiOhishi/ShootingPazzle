#include <fstream>

#include "EditorStage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "NormalFloor.h"
#include "TurnFloor.h"
#include "GameUtility.h"

EditorStage::~EditorStage()
{
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) delete floors[i];
	}
	floors.clear();
}

void EditorStage::LoadStage(std::string filename)
{
	//�f�[�^������
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) delete floors[i];
	}
	floors.clear();

	//GameUtility�ɃX�e�[�W�T�C�Y��n��
	GameUtility::SetPStageSize(&stageSize);

	std::ifstream file;
	//�o�C�i�����[�h�ŊJ��
	file.open(filename, std::ios_base::binary);
	//�J���Ȃ�������u���b�N�Ȃ����~���l�߃f�[�^�𐶐�
	if (!file.is_open()) {
		stageSize.x = 20;
		stageSize.y = 20;
		startLaneZ = stageSize.y - 2;
		//�S�}�X�ɏ��u���b�N(�m�[�}��)
		for (int i = 0; i < stageSize.y; i++) {
			for (int j = 0; j < stageSize.x; j++) {
				StageVec2 pos = { i,j };
				NormalFloor* newFloor = new NormalFloor;
				newFloor->Initialize(pos);
				floors.emplace_back(newFloor);
			}
		}
		//�������蔻�菉����
		floorCollision.distance = 0;
		floorCollision.normal = { 0,1,0 };	//�@�������͏�
		return;
	}

	//�X�e�[�W���i�[
	StageHeader header;
	file.read((char*)&header, sizeof(header));
	stageSize.x = header.width;
	stageSize.y = header.depth;
	startLaneZ = header.startLineZ;

	//�u���b�N���i�[
	for (int i = 0; i < header.blockCount; i++) {
		StageBlock object;
		file.read((char*)&object, sizeof(object));

		StageVec2 pos = { object.stagePosX, object.stagePosY };

		//SquareBlock
		if (object.type == 0) {
			SquareBlock* newBlock = new SquareBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LT
		else if (object.type == 1) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTTOP);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RT
		else if (object.type == 2) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTTOP);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock LB
		else if (object.type == 3) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTBOTTOM);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
		//TriangleBlock RB
		else if (object.type == 4) {
			TriangleBlock* newBlock = new TriangleBlock;
			newBlock->Initialize(pos, sphereRadius);
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTBOTTOM);
			newBlock->SetBreakupCount(object.breakupCount);
			blocks.emplace_back(newBlock);
		}
	}

	//�����i�[
	//�X�e�[�W�f�[�^�Ŏw�肳�ꂽ�ӏ��͎w�肳�ꂽ�f�[�^���i�[�A����ȊO�̓m�[�}���u���b�N
	for (int i = 0; i < header.floorCount; i++) {
		StageFloor object;
		file.read((char*)&object, sizeof(object));

		StageVec2 pos = { object.stagePosX, object.stagePosY };

		if (object.type == 0) {
			NormalFloor* newFloor = new NormalFloor;
			newFloor->Initialize(pos);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 1) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_LEFT);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 2) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_RIGHT);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 3) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_UP);
			floors.emplace_back(newFloor);
		}
		else if (object.type == 4) {
			TurnFloor* newFloor = new TurnFloor;
			newFloor->Initialize(pos);
			newFloor->SetTurnType(TURNTYPE_DOWN);
			floors.emplace_back(newFloor);
		}
	}

	file.close();

	//�������蔻�菉����
	floorCollision.distance = 0;
	floorCollision.normal = { 0,1,0 };	//�@�������͏�
}

void EditorStage::Update()
{
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) floors[i]->Update();
	}

	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Update();
	}
}

void EditorStage::Draw()
{
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) floors[i]->Draw();
	}

	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Draw();
	}
}

void EditorStage::AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount)
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
	else {
		TriangleBlock* newBlock = new TriangleBlock;
		newBlock->Initialize(stagePos, ONE_CELL_LENGTH / 2);
		newBlock->SetBreakupCount(breakupCount);
		if (blockType == BLOCKTYPE_TRIANGLE_NO_LEFTTOP) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTTOP);
		}
		else if (blockType == BLOCKTYPE_TRIANGLE_NO_RIGHTTOP) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTTOP);
		}
		else if (blockType == BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_LEFTBOTTOM);
		}
		else if (blockType == BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM) {
			newBlock->SetTriangleType(TRIANGLETYPE_NO_RIGHTBOTTOM);
		}
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

void EditorStage::AddFloor(const StageVec2& stagePos, int floorType)
{
	//���ɏ��u���b�N���z�u����Ă����烊�^�[��
	if (CheckExistFloor(stagePos) != -1) {
		return;
	}

	//��ނ��Ƃɏ��u���b�N�ǉ�
	if (floorType == FLOORTYPE_NORMAL) {
		NormalFloor* newFloor = new NormalFloor;
		newFloor->Initialize(stagePos);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_LEFT) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_LEFT);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_RIGHT) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_RIGHT);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_UP) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_UP);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_DOWN) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_DOWN);
		floors.emplace_back(newFloor);
	}
}

void EditorStage::DeleteFloor(const StageVec2& stagePos)
{
	//���u���b�N���z�u����Ă��Ȃ������烊�^�[��
	int deleteIndex = CheckExistFloor(stagePos);
	if (deleteIndex == -1) {
		return;
	}

	//���u���b�N�폜
	if (floors[deleteIndex]) delete floors[deleteIndex];
	floors.erase(floors.begin() + deleteIndex);
}

int EditorStage::CheckExistFloor(const StageVec2& stagePos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(stagePos, &x, &z);

	for (int i = 0; i < floors.size(); i++) {
		if (x == floors[i]->GetPosition().x && z == floors[i]->GetPosition().z) {
			return i;
		}
	}

	return -1;
}
