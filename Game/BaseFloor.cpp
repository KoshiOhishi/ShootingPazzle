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
	if (firstEffectEndTime == -1) {
		firstEffectEndTime = 1700 + rand() % 200;
	}

	float y = (float)Easing::GetEaseValue(firstEffectType, 300, -ONE_CELL_LENGTH / 2, timer, 250, firstEffectEndTime);

	Vector3 nPos = object.GetPosition();
	object.SetPosition({ nPos.x, y, nPos.z });
}

void BaseFloor::UpdateClearEffect(const Timer& timer)
{
	//初回だけエフェクトのスタート時間決め
	if (clearEffectStartTime == -1) {
		clearEffectStartTime = 500 + rand() % 1500;
	}

	//タイマーの値がclearEffectStartTimeを超えていたらブロックを落とす
	if ((double)timer.GetNowTime() >= clearEffectStartTime) {
		Vector3 nPos = object.GetPosition();
		float sub = (timer.GetNowTime() - clearEffectStartTime) * 0.01f;
		object.SetPosition({ nPos.x, nPos.y - sub, nPos.z });
	}
}

void BaseFloor::SetStagePos(const StageVec2& pos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(pos, &x, &z);

	object.SetPosition({ x, -ONE_CELL_LENGTH / 2, z }); 
}
