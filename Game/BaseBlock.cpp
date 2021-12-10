#include "BaseBlock.h"
#include "GameUtility.h"

void BaseBlock::SetStagePos(const StageVec2& pos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(pos, &x, &z);

	object.SetPosition(x, ONE_CELL_LENGTH / 2, z);
}

void BaseBlock::SetBreakupCount(unsigned short breakupCount)
{
	this->breakupCount = breakupCount;
	//最初から0の時は壊れないブロック
	isBreakable = breakupCount > 0;

	//とりあえず壊れるブロックは色変えとく
	if (breakupCount == 2) {
		//緑
		object.SetColor(0.5f, 1, 0.5f, 1);
	}
	else if (breakupCount == 1) {
		//赤
		object.SetColor(1, 0.5f, 0.5f, 1);
	}
	else if (breakupCount == 0) {
		//白
		object.SetColor(1, 1, 1, 1);
	}
}