#include "GameUtility.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"

//現在のフェーズ
int GameUtility::nowPhase;


void GameUtility::StaticInitialize()
{
	MyBullet::CreateModel();
	TriangleBlock::CreateModel();
	SquareBlock::CreateModel();
}

const StagePos& GameUtility::CalcWorldPos2StagePos(float worldX, float worldZ, float stageWidth, float stageDepth)
{
	StagePos result;
	result.x = (worldX + stageWidth * ONE_CELL_LENGTH / 2) / ONE_CELL_LENGTH;
	result.y = -(worldZ - stageDepth * ONE_CELL_LENGTH / 2) / ONE_CELL_LENGTH;

	return result;
}

void GameUtility::CalcStagePos2WorldPos(const StagePos& stagePos, float stageWidth, float stageDepth, float* dstWorldX, float* dstWorldZ)
{
	if (dstWorldX) {
		*dstWorldX = stagePos.x * ONE_CELL_LENGTH - stageWidth * ONE_CELL_LENGTH / 2;
		*dstWorldX += ONE_CELL_LENGTH / 2;
	}
	if (dstWorldZ) {
		*dstWorldZ = -stagePos.y * ONE_CELL_LENGTH + stageDepth * ONE_CELL_LENGTH / 2;
		*dstWorldZ -= ONE_CELL_LENGTH / 2;
	}
}