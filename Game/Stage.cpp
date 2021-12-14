#include <fstream>

#include "Stage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "NormalFloor.h"
#include "HoleFloor.h"
#include "TurnFloor.h"
#include "GameUtility.h"

Stage::~Stage()
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

void Stage::LoadStage(std::string filename)
{
	NormalFloor::StaticInitialize();
	TurnFloor::StaticInitialize();
	HoleFloor::StaticInitialize();

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
	//�J���Ȃ�������u���b�N�Ȃ��f�[�^�𐶐�
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

	InsertHole();

	//�������蔻�菉����
	floorCollision.distance = 0;
	floorCollision.normal = { 0,1,0 };	//�@�������͏�
}

void Stage::Update()
{
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) floors[i]->Update();
	}

	for (int i = 0; i < blocks.size(); i++) {
		//�u���b�N�����Ă�����폜
		if (blocks[i]->isBreakup()) {
			delete blocks[i];
			blocks.erase(blocks.begin() + i);
			i--;
			continue;
		}
		if (blocks[i]) blocks[i]->Update();
	}
}

void Stage::Draw()
{
	NormalFloor::Draw();
	TurnFloor::Draw();

	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Draw();
	}

	NormalFloor::ResetIndex();
	for (int i = 0; i < 4; i++) {
		TurnFloor::ResetIndex(i);
	}
}

void Stage::InsertHole()
{
	for (unsigned short i = 0; i < stageSize.y; i++) {
		for (unsigned short j = 0; j < stageSize.x; j++) {
			//���̃}�X�ɂȂɂ����炪�z�u����Ă��Ȃ���Α}��
			if (CheckExistFloor({ i,j }) == -1) {
				HoleFloor* newFloor = new HoleFloor;
				newFloor->Initialize({ i,j });
				floors.emplace_back(newFloor);
			}
		}
	}
}

int Stage::CheckExistFloor(const StageVec2& stagePos)
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
