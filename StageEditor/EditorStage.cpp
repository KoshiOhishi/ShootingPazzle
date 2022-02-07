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
#include "Command.h"

EditorStage::~EditorStage()
{
	blocks.clear();
	floors.clear();
}

void EditorStage::Initialize()
{
	//モデルセット
	for (int i = 0; i < _countof(iodSquareBlock); i++) {
		iodSquareBlock[i].Initialize();
		iodSquareBlock[i].SetObjModel(SquareBlock::GetModel(i));
	}
	for (int i = 0; i < _countof(iodTriangleBlock); i++) {
		iodTriangleBlock[i].Initialize();
		iodTriangleBlock[i].SetObjModel(TriangleBlock::GetModel(i));
	}
	iodNormalFloor.Initialize();
	iodNormalFloor.SetObjModel(NormalFloor::GetModel());
	for (int i = 0; i < _countof(iodSwitchFloor); i++) {
		iodSwitchFloor[i].Initialize();
		iodSwitchFloor[i].SetObjModel(SwitchFloor::GetModel(i));
	}
	for (int i = 0; i < _countof(iodTurnFloor); i++) {
		iodTurnFloor[i].Initialize();
		iodTurnFloor[i].SetObjModel(TurnFloor::GetModel(i));
	}
	iodBreakFloor.Initialize();
	iodBreakFloor.SetObjModel(BreakFloor::GetModel());
}

void EditorStage::LoadStage(std::string filename)
{
	//データ初期化
	blocks.clear();
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
				newFloor->SetPInstancingObjectDraw(&iodNormalFloor);
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

	//描画用オブジェクト更新
	iodNormalFloor.Update();
	for (int i = 0; i < _countof(iodSwitchFloor); i++) {
		iodSwitchFloor[i].Update();
	}
	for (int i = 0; i < _countof(iodTurnFloor); i++) {
		iodTurnFloor[i].Update();
	}
	iodBreakFloor.Update();
	for (int i = 0; i < _countof(iodSquareBlock); i++) {
		iodSquareBlock[i].Update();
	}
	for (int i = 0; i < _countof(iodTriangleBlock); i++) {
		iodTriangleBlock[i].Update();
	}
}

void EditorStage::Draw()
{
	iodNormalFloor.Draw();
	for (int i = 0; i < _countof(iodSwitchFloor); i++) {
		iodSwitchFloor[i].Draw();
	}
	for (int i = 0; i < _countof(iodTurnFloor); i++) {
		iodTurnFloor[i].Draw();
	}
	iodBreakFloor.Draw();

	for (int i = 0; i < _countof(iodSquareBlock); i++) {
		iodSquareBlock[i].Draw();
	}
	for (int i = 0; i < _countof(iodTriangleBlock); i++) {
		iodTriangleBlock[i].Draw();
	}
}

void EditorStage::AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount, int blockColor, bool addCommand)
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
		newBlock->SetPInstancingObjectDraw(iodSquareBlock);
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
		newBlock->SetPInstancingObjectDraw(iodTriangleBlock);
		blocks.emplace_back(newBlock);
	}

	if (addCommand) {
		//コマンド追加
		CommandDetail data;
		data.commandType = COMMAND_TYPE_ADD;
		data.args1 = stagePos.x;
		data.args2 = stagePos.y;
		data.objectType = OBJECTTYPE_BLOCK;
		data.blockFloorType = blockType;
		data.breakupCount = breakupCount;
		data.colorType = blockColor;
		Command::AddCommand(data);
	}
}

void EditorStage::DeleteBlock(const StageVec2& stagePos, bool addCommand)
{
	//ブロックが配置されていなかったらリターン
	int deleteIndex = CheckExistBlock(stagePos);
	if (deleteIndex == -1) {
		return;
	}

	if (addCommand) {
		//コマンド追加
		CommandDetail data;
		data.commandType = COMMAND_TYPE_DELETE;
		data.args1 = stagePos.x;
		data.args2 = stagePos.y;
		data.objectType = OBJECTTYPE_BLOCK;
		data.blockFloorType = blocks[deleteIndex]->GetBlockType();
		data.breakupCount = blocks[deleteIndex]->GetBreakupCount();
		data.colorType = blocks[deleteIndex]->GetBlockColor();
		Command::AddCommand(data);
	}

	//ブロック削除
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

void EditorStage::AddFloor(const StageVec2& stagePos, int floorType, bool addCommand)
{
	//既に床ブロックが配置されていたらリターン
	if (CheckExistFloor(stagePos) != -1) {
		return;
	}

	//種類ごとに床ブロック追加
	if (floorType == FLOORTYPE_NORMAL) {
		NormalFloor* newFloor = new NormalFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetPInstancingObjectDraw(&iodNormalFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_LEFT) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_LEFT);
		newFloor->SetPInstancingObjectDraw(iodTurnFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_RIGHT) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_RIGHT);
		newFloor->SetPInstancingObjectDraw(iodTurnFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_UP) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_UP);
		newFloor->SetPInstancingObjectDraw(iodTurnFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_TURN_DOWN) {
		TurnFloor* newFloor = new TurnFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetTurnType(TURNTYPE_DOWN);
		newFloor->SetPInstancingObjectDraw(iodTurnFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_BREAK) {
		BreakFloor* newFloor = new BreakFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetPInstancingObjectDraw(&iodBreakFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_NONE) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_NONE);
		newFloor->SetPInstancingObjectDraw(iodSwitchFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_RED) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_RED);
		newFloor->SetPInstancingObjectDraw(iodSwitchFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_BLUE) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_BLUE);
		newFloor->SetPInstancingObjectDraw(iodSwitchFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_YELLOW) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_YELLOW);
		newFloor->SetPInstancingObjectDraw(iodSwitchFloor);
		floors.emplace_back(newFloor);
	}
	else if (floorType == FLOORTYPE_SWITCH_GREEN) {
		SwitchFloor* newFloor = new SwitchFloor;
		newFloor->Initialize(stagePos);
		newFloor->SetSwitchColor(SWITCH_COLOR_GREEN);
		newFloor->SetPInstancingObjectDraw(iodSwitchFloor);
		floors.emplace_back(newFloor);
	}

	if (addCommand) {
		//コマンド追加
		CommandDetail data;
		data.commandType = COMMAND_TYPE_ADD;
		data.args1 = stagePos.x;
		data.args2 = stagePos.y;
		data.objectType = OBJECTTYPE_FLOOR;
		data.blockFloorType = floorType;
		Command::AddCommand(data);
	}
}

void EditorStage::DeleteFloor(const StageVec2& stagePos, bool addCommand)
{
	//床ブロックが配置されていなかったらリターン
	int deleteIndex = CheckExistFloor(stagePos);
	if (deleteIndex == -1) {
		return ;
	}

	if (addCommand) {
		//コマンド追加
		CommandDetail data;
		data.commandType = COMMAND_TYPE_DELETE;
		data.args1 = stagePos.x;
		data.args2 = stagePos.y;
		data.objectType = OBJECTTYPE_FLOOR;
		data.blockFloorType = floors[deleteIndex]->GetFloorType();
		Command::AddCommand(data);
	}

	//床ブロック削除
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