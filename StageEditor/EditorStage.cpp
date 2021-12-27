#include <fstream>

#include "EditorStage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "NormalFloor.h"
#include "TurnFloor.h"
#include "HoleFloor.h"
#include "BreakFloor.h"
#include "SwitchFloor.h"
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

	//データ初期化
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) delete blocks[i];
	}
	blocks.clear();
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) delete floors[i];
	}
	floors.clear();

	//GameUtilityにステージサイズを渡す
	GameUtility::SetPStageSize(&stageSize);

	std::ifstream file;
	//バイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//開けなかったらブロックなし床敷き詰めデータを生成
	if (!file.is_open()) {
		stageSize.x = 20;
		stageSize.y = 20;
		startLaneZ = stageSize.y - 2;
		//全マスに床ブロック(ノーマル)
		for (int i = 0; i < stageSize.y; i++) {
			for (int j = 0; j < stageSize.x; j++) {
				StageVec2 pos = { i,j };
				NormalFloor* newFloor = new NormalFloor;
				newFloor->Initialize(pos);
				floors.emplace_back(newFloor);
			}
		}
		//床当たり判定初期化
		floorCollision.distance = 0;
		floorCollision.normal = { 0,1,0 };	//法線方向は上
		return;
	}

	//ステージ情報格納
	StageHeader header;
	file.read((char*)&header, sizeof(header));
	stageSize.x = header.width;
	stageSize.y = header.depth;
	startLaneZ = header.startLineZ;

	//ブロック情報格納
	for (int i = 0; i < header.blockCount; i++) {
		StageBlock object;
		file.read((char*)&object, sizeof(object));

		StageVec2 pos = { object.stagePosX, object.stagePosY };
		AddBlock(pos, object.type, object.breakupCount, object.blockColor);
	}

	//床情報格納
	//ステージデータで指定された箇所は指定されたデータを格納、それ以外はノーマルブロック
	for (int i = 0; i < header.floorCount; i++) {
		StageFloor object;
		file.read((char*)&object, sizeof(object));

		StageVec2 pos = { object.stagePosX, object.stagePosY };
		AddFloor(pos, object.type);
	}

	file.close();

	//床当たり判定初期化
	floorCollision.distance = 0;
	floorCollision.normal = { 0,1,0 };	//法線方向は上
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
	NormalFloor::Draw();
	TurnFloor::Draw();
	BreakFloor::Draw();
	SwitchFloor::Draw();

	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i]) blocks[i]->Draw();
	}
}

void EditorStage::EndDraw()
{
	NormalFloor::EndDraw();
	TurnFloor::EndDraw();
	BreakFloor::EndDraw();
	SwitchFloor::EndDraw();
}

void EditorStage::AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount, int blockColor)
{
	//既にブロックが配置されていたらリターン
	if (CheckExistBlock(stagePos) != -1) {
		return;
	}

	//種類ごとにブロック追加
	if (blockType == BLOCKTYPE_SQUARE) {
		SquareBlock* newBlock = new SquareBlock;
		newBlock->Initialize(stagePos, ONE_CELL_LENGTH / 2);
		newBlock->SetBreakupCount(breakupCount);
		newBlock->SetBlockColor(blockColor);
		blocks.emplace_back(newBlock);
	}
	else {
		TriangleBlock* newBlock = new TriangleBlock;
		newBlock->Initialize(stagePos, ONE_CELL_LENGTH / 2);
		newBlock->SetBreakupCount(breakupCount);
		newBlock->SetBlockColor(blockColor);
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
	//ブロックが配置されていなかったらリターン
	int deleteIndex = CheckExistBlock(stagePos);
	if (deleteIndex == -1) {
		return;
	}

	//ブロック削除
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
	//既に床ブロックが配置されていたらリターン
	if (CheckExistFloor(stagePos) != -1) {
		return;
	}

	//種類ごとに床ブロック追加
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
	else if (floorType == FLOORTYPE_BREAK) {
		BreakFloor* newFloor = new BreakFloor;
		newFloor->Initialize(stagePos);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_WHITE) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_WHITE);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_RED) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_RED);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_BLUE) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_BLUE);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_YELLOW) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_YELLOW);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_GREEN) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_GREEN);
		floors.emplace_back(newFloor);
	}
}

void EditorStage::DeleteFloor(const StageVec2& stagePos)
{
	//床ブロックが配置されていなかったらリターン
	int deleteIndex = CheckExistFloor(stagePos);
	if (deleteIndex == -1) {
		return;
	}

	//床ブロック削除
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
