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

const StageVec2& GameUtility::CalcWorldPos2StagePos(float worldX, float worldZ, const StageVec2& stageSize)
{
	StageVec2 result;
	result.x = (worldX + stageSize.x * ONE_CELL_LENGTH / 2) / ONE_CELL_LENGTH;
	result.y = -(worldZ - stageSize.y * ONE_CELL_LENGTH / 2) / ONE_CELL_LENGTH;

	return result;
}

void GameUtility::CalcStagePos2WorldPos(const StageVec2& stagePos, const StageVec2& stageSize, float* dstWorldX, float* dstWorldZ)
{
	if (dstWorldX) {
		*dstWorldX = stagePos.x * ONE_CELL_LENGTH - stageSize.x * ONE_CELL_LENGTH / 2;
		*dstWorldX += ONE_CELL_LENGTH / 2;
	}
	if (dstWorldZ) {
		*dstWorldZ = -stagePos.y * ONE_CELL_LENGTH + stageSize.y * ONE_CELL_LENGTH / 2;
		*dstWorldZ -= ONE_CELL_LENGTH / 2;
	}
}