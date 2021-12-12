#include "GameUtility.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "NormalFloor.h"
#include "TurnFloor.h"

//現在のフェーズ
int GameUtility::nowPhase;
StageVec2* GameUtility::pStageSize = nullptr;

void GameUtility::StaticInitialize()
{
	MyBullet::CreateModel();
	TriangleBlock::CreateModel();
	SquareBlock::CreateModel();
	NormalFloor::CreateModel();
	TurnFloor::CreateModel();
}

const StageVec2& GameUtility::CalcWorldPos2StagePos(float worldX, float worldZ)
{
	assert(pStageSize);

	StageVec2 result;
	result.x = (worldX + pStageSize->x * ONE_CELL_LENGTH / 2) / ONE_CELL_LENGTH;
	result.y = -(worldZ - pStageSize->y * ONE_CELL_LENGTH / 2) / ONE_CELL_LENGTH;

	return result;
}

void GameUtility::CalcStagePos2WorldPos(const StageVec2& stagePos, float* dstWorldX, float* dstWorldZ)
{
	assert(pStageSize);

	if (dstWorldX) {
		*dstWorldX = stagePos.x * ONE_CELL_LENGTH - pStageSize->x * ONE_CELL_LENGTH / 2;
		*dstWorldX += ONE_CELL_LENGTH / 2;
	}
	if (dstWorldZ) {
		*dstWorldZ = -stagePos.y * ONE_CELL_LENGTH + pStageSize->y * ONE_CELL_LENGTH / 2;
		*dstWorldZ -= ONE_CELL_LENGTH / 2;
	}
}