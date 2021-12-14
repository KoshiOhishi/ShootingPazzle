#include "BaseFloor.h"
#include "GameUtility.h"

void BaseFloor::SetStagePos(const StageVec2& pos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(pos, &x, &z);

	object.SetPosition({ x, -ONE_CELL_LENGTH / 2, z });
}
