#include "GameUtility.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "NormalFloor.h"
#include "TurnFloor.h"
#include "BreakFloor.h"
#include "SwitchFloor.h"
#include "HoleFloor.h"

int GameUtility::nowPhase;
StageVec2* GameUtility::pStageSize = nullptr;
int GameUtility::stageColor = STAGE_COLOR_NONE;
std::string GameUtility::nowStagePath = "";
bool GameUtility::isPause = false;
const Vector4 GameUtility::COLOR_VALUE[5] = {
	{0.75,0.75,0.75,1},		//ƒOƒŒ[
	{1,0.25,0.25,1},		//Ô
	{0.25,0.25,1,1},		//Â
	{1,0.75,0.25,1},		//‰©
	{0.25,1,0.25,1}			//—Î
};
const Vector4 GameUtility::COLOR_VALUE_BREAKABLE[5] = {
	{1,1,1,1},				//”’
	{1,0.66,0.66,1},		//”’‚Á‚Û‚¢Ô
	{0.66,0.66,1,1},		//”’‚Á‚Û‚¢Â
	{1,1,0.66,1},			//”’‚Á‚Û‚¢‰©
	{0.66,1,0.66,1}			//”’‚Á‚Û‚¢—Î
};

void GameUtility::StaticInitialize()
{
	MyBullet::StaticInitialize();
	SquareBlock::StaticInitialize();
	TriangleBlock::StaticInitialize();
	NormalFloor::StaticInitialize();
	TurnFloor::StaticInitialize();
	BreakFloor::StaticInitialize();
	SwitchFloor::StaticInitialize();
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