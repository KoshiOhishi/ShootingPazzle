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
	if ((double)timer.GetNowTime() >= clearEffectStartTime && timer.GetIsStart()) {
		Vector3 nPos = object.GetPosition();
		float sub = (timer.GetNowTime() - clearEffectStartTime) * 0.01f;
		object.SetPosition({ nPos.x, nPos.y - sub, nPos.z });
	}
}

void BaseBlock::UpdateColor()
{
	//白は通過可能にならない
	if (blockColor == BLOCK_COLOR_NONE) {
		return;
	}

	//ステージ色が変わったとき　かつ
	//自分の色が含まれていたらタイマー開始
	if (prevStageColor != GameUtility::GetStageColor() &&
		(GameUtility::GetStageColor() == blockColor ||
			prevStageColor == blockColor)) {
		changeColorTimer.Reset();
		changeColorTimer.Start();
	}

	changeColorTimer.Update();

	//色変更中の処理
	if (changeColorTimer.GetIsStart()) {
		double	start = changeColorTimer.GetStartTime(),
			end = changeColorTimer.GetEndTime(),
			middle = (end - start) / 4;	//ちょっと早い位置で

		float r = 0, g = 0, b = 0, a = 0;
		//他の色→ブロックの色
		if (GameUtility::GetStageColor() == blockColor) {
			//前の色→白
			if (changeColorTimer.GetNowTime() < middle) {
				r = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].x, 2, changeColorTimer, start, middle);
				g = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].y, 2, changeColorTimer, start, middle);
				b = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].z, 2, changeColorTimer, start, middle);
				a = 1;
			}
			//白→今の色
			else {
				r = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].x, changeColorTimer, middle, end);
				g = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].y, changeColorTimer, middle, end);
				b = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].z, changeColorTimer, middle, end);
				a = Easing::GetEaseValue(EASE_OUTEXPO, 1, 0.5, changeColorTimer, middle, end);
			}
		}
		//ブロックの色→他の色
		else {
			//前の色→白
			if (changeColorTimer.GetNowTime() < middle) {
				r = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].x, 2, changeColorTimer, start, middle);
				g = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].y, 2, changeColorTimer, start, middle);
				b = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].z, 2, changeColorTimer, start, middle);
				a = Easing::GetEaseValue(EASE_OUTEXPO, 0.5, 1, changeColorTimer, start, middle);
			}
			//白→今の色
			else {
				r = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].x, changeColorTimer, middle, end);
				g = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].y, changeColorTimer, middle, end);
				b = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].z, changeColorTimer, middle, end);
				a = 1;
			}
		}

		object.SetColor({ r,g,b,a });
	}
	else {
		Vector4 tmp = GameUtility::COLOR_VALUE[blockColor];
		//ステージの色とブロックの色が一致していたら透過
		if (GameUtility::GetStageColor() == blockColor) {
			object.SetColor({ tmp.x, tmp.y, tmp.z, 0.5f });
		}
		else {
			object.SetColor({ tmp.x, tmp.y, tmp.z, 1 });
		}
	}

	//前フレームの色に今の色を入れておく
	prevStageColor = GameUtility::GetStageColor();
}