#include <fstream>

#include "Stage.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "NormalFloor.h"
#include "HoleFloor.h"
#include "TurnFloor.h"
#include "BreakFloor.h"
#include "GameUtility.h"
#include "DebugText.h"

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
	//BaseBlockにポインタセット
	BaseBlock::SetPStage(this);
	targetBlockCount = 0;

	//エフェクトは最初の読み込みのみ
	if (GameUtility::GetNowPhase() == PHASE_FIRSTEFFECT) {
		firstEffectTimer.SetTimer(0, 3500);
		firstEffectTimer.Start();
	}

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
	//開けなかったらブロックなしデータを生成
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
		AddBlock(pos, object.type, object.breakupCount);
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

	InsertHole();

	//床当たり判定初期化
	floorCollision.distance = 0;
	floorCollision.normal = { 0,1,0 };	//法線方向は上
}

void Stage::Update()
{
	//出現エフェクト
	UpdateFirstEffect();

	//オブジェクト更新
	for (int i = 0; i < floors.size(); i++) {
		if (floors[i]) floors[i]->Update();
	}

	for (int i = 0; i < blocks.size(); i++) {
		//ブロックが壊れていたら削除
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
	BreakFloor::Draw();

	for (int i = 0; i < blocks.size(); i++) {
		blocks[i]->Draw();
	}

	NormalFloor::ResetIndex();
	for (int i = 0; i < 4; i++) {
		TurnFloor::ResetIndex(i);
	}
	BreakFloor::ResetIndex();
}

void Stage::UpdateFirstEffect()
{
	if (GameUtility::GetNowPhase() != PHASE_FIRSTEFFECT) {
		return;
	}

	firstEffectTimer.Update();

	//ブロックが空から降ってくる演出
	for (int i = 0; i < blocks.size(); i++) {
		blocks[i]->UpdateFirstEffect(firstEffectTimer);
	}

	//床が下から出てくる演出
	for (int i = 0; i < floors.size(); i++) {
		floors[i]->UpdateFirstEffect(firstEffectTimer);
	}

	if (firstEffectTimer.GetIsEnd()) {
		GameUtility::SetNowPhase(PHASE_SETPOS);
	}
}

void Stage::InsertHole()
{
	for (unsigned short i = 0; i < stageSize.y; i++) {
		for (unsigned short j = 0; j < stageSize.x; j++) {
			//そのマスになにかしらが配置されていなければ挿入
			if (CheckExistFloor({ i,j }) == -1) {
				HoleFloor* newFloor = new HoleFloor;
				newFloor->Initialize({ i,j });
				floors.emplace_back(newFloor);
			}
		}
	}
}

void Stage::AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount)
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

	//破壊できるブロックなら目標ブロックカウント増加
	if (breakupCount > 0) {
		targetBlockCount++;
	}
}

void Stage::DeleteBlock(const StageVec2& stagePos)
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

int Stage::CheckExistBlock(const StageVec2& stagePos)
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

void Stage::AddFloor(const StageVec2& stagePos, int floorType)
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
}

void Stage::DeleteFloor(const StageVec2& stagePos)
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
