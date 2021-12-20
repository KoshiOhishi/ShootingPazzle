#include "BaseFloor.h"
#include "GameUtility.h"
#include "Easing.h"

void BaseFloor::UpdateFirstEffect(const Timer& timer)
{
	//初回だけエフェクトの種類決め
	if (firstEffectType == -1) {
		//easeOut系の中からランダムで。(1,4,7,10,...)
		firstEffectType = rand() % 7;
		firstEffectType = 3 * firstEffectType + 1;
	}
	if (effectEndTime == -1) {
		effectEndTime = 1700 + rand() % 200;
	}

	float y = (float)Easing::GetEaseValue(firstEffectType, 300, -ONE_CELL_LENGTH / 2, timer, 250, effectEndTime);

	Vector3 nPos = object.GetPosition();
	object.SetPosition({ nPos.x, y, nPos.z });
}

void BaseFloor::SetStagePos(const StageVec2& pos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(pos, &x, &z);

	object.SetPosition({ x, -ONE_CELL_LENGTH / 2, z });
}
