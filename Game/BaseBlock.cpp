#include "BaseBlock.h"
#include "GameUtility.h"
#include "Easing.h"

Stage* BaseBlock::pStage = nullptr;

void BaseBlock::SetStagePos(const StageVec2& pos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(pos, &x, &z);

	object.SetPosition({ x, ONE_CELL_LENGTH / 2, z });
}

void BaseBlock::SetBreakupCount(unsigned short breakupCount)
{
	this->breakupCount = breakupCount;
	//最初から0の時は壊れないブロック
	isBreakable = breakupCount > 0;
}

void BaseBlock::SetBlockColor(int blockColor)
{
	this->blockColor = blockColor;

	//色設定
	switch (blockColor) {
	case BLOCK_COLOR_NONE:
		if (breakupCount > 0) {
			object.SetColor({ 1, 1, 1, 1 });
		}
		else {
			object.SetColor({ 0.5f, 0.5f, 0.5f, 1 });
		}
		break;
	case BLOCK_COLOR_RED:
		if (breakupCount > 0) {
			object.SetColor({ 1, 0.66f, 0.66f, 1 });
		}
		else {
			object.SetColor({ 1, 0.25f, 0, 1 });
		}
		break;
	case BLOCK_COLOR_BLUE:
		if (breakupCount > 0) {
			object.SetColor({ 0.66f, 0.66f, 1, 1 });
		}
		else {
			object.SetColor({ 0, 0.25f, 1, 1 });
		}
		break;
	case BLOCK_COLOR_YELLOW:
		if (breakupCount > 0) {
			object.SetColor({ 1, 1, 0.66f, 1 });
		}
		else {
			object.SetColor({ 1, 0.75f, 0, 1 });
		}
		break;
	case BLOCK_COLOR_GREEN:
		if (breakupCount > 0) {
			object.SetColor({ 0.66f, 1, 0.66f, 1 });
		}
		else {
			object.SetColor({ 0, 1, 0.25f, 1 });
		}
		break;
	}
}

void BaseBlock::UpdateFirstEffect(const Timer& timer)
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
	
	double y = Easing::GetEaseValue(firstEffectType, 300, ONE_CELL_LENGTH / 2, timer, 500, firstEffectEndTime);

	Vector3 nPos = object.GetPosition();
	object.SetPosition({ nPos.x, (float)y, nPos.z });
}

void BaseBlock::UpdateClearEffect(const Timer& timer)
{
	//初回だけエフェクトのスタート時間決め
	if (clearEffectStartTime == -1) {
		clearEffectStartTime = 1000 + rand() % 1500;
	}

	//タイマーの値がclearEffectStartTimeを超えていたらブロックを落とす
	if ((double)timer.GetNowTime() >= clearEffectStartTime) {
		Vector3 nPos = object.GetPosition();
		float sub = (timer.GetNowTime() - clearEffectStartTime) * 0.01f;
		object.SetPosition({ nPos.x, nPos.y - sub, nPos.z });
	}
}

void BaseBlock::UpdateColor()
{
	//黒は通過可能にならない
	if (blockColor == BLOCK_COLOR_NONE) {
		return;
	}

	Vector4 tmp = object.GetColor();
	//ステージの色とブロックの色が一致していたら透過
	if (GameUtility::GetStageColor() == blockColor) {
		object.SetColor({ tmp.x, tmp.y, tmp.z, 0.65f });
	}
	else {
		object.SetColor({ tmp.x, tmp.y, tmp.z, 1 });
	}
}