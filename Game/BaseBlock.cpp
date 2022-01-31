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
	//�ŏ�����0�̎��͉��Ȃ��u���b�N
	isBreakable = breakupCount > 0;
}

void BaseBlock::SetBlockColor(int blockColor)
{
	this->blockColor = blockColor;
}

void BaseBlock::UpdateFirstEffect(const Timer& timer)
{
	//���񂾂��G�t�F�N�g�̎�ތ���
	if (firstEffectType == -1) {
		//easeOut�n�̒����烉���_���ŁB(1,4,7,10,...)
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
	//���񂾂��G�t�F�N�g�̃X�^�[�g���Ԍ���
	if (clearEffectStartTime == -1) {
		clearEffectStartTime = 1000 + rand() % 1500;
	}

	//�^�C�}�[�̒l��clearEffectStartTime�𒴂��Ă�����u���b�N�𗎂Ƃ�
	if ((double)timer.GetNowTime() >= clearEffectStartTime && timer.GetIsStart()) {
		Vector3 nPos = object.GetPosition();
		float sub = (timer.GetNowTime() - clearEffectStartTime) * 0.01f;
		object.SetPosition({ nPos.x, nPos.y - sub, nPos.z });
	}
}

void BaseBlock::UpdateColor()
{
	//���͒ʉ߉\�ɂȂ�Ȃ�
	if (blockColor == BLOCK_COLOR_NONE) {
		return;
	}

	//�X�e�[�W�F���ς�����Ƃ��@����
	//�����̐F���܂܂�Ă�����^�C�}�[�J�n
	if (prevStageColor != GameUtility::GetStageColor() &&
		(GameUtility::GetStageColor() == blockColor ||
			prevStageColor == blockColor)) {
		changeColorTimer.Reset();
		changeColorTimer.Start();
	}

	changeColorTimer.Update();

	//�F�ύX���̏���
	if (changeColorTimer.GetIsStart()) {
		double	start = changeColorTimer.GetStartTime(),
			end = changeColorTimer.GetEndTime(),
			middle = (end - start) / 4;	//������Ƒ����ʒu��

		float r = 0, g = 0, b = 0, a = 0;
		//���̐F���u���b�N�̐F
		if (GameUtility::GetStageColor() == blockColor) {
			//�O�̐F����
			if (changeColorTimer.GetNowTime() < middle) {
				r = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].x, 2, changeColorTimer, start, middle);
				g = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].y, 2, changeColorTimer, start, middle);
				b = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].z, 2, changeColorTimer, start, middle);
				a = 1;
			}
			//�������̐F
			else {
				r = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].x, changeColorTimer, middle, end);
				g = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].y, changeColorTimer, middle, end);
				b = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[blockColor].z, changeColorTimer, middle, end);
				a = Easing::GetEaseValue(EASE_OUTEXPO, 1, 0.5, changeColorTimer, middle, end);
			}
		}
		//�u���b�N�̐F�����̐F
		else {
			//�O�̐F����
			if (changeColorTimer.GetNowTime() < middle) {
				r = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].x, 2, changeColorTimer, start, middle);
				g = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].y, 2, changeColorTimer, start, middle);
				b = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[blockColor].z, 2, changeColorTimer, start, middle);
				a = Easing::GetEaseValue(EASE_OUTEXPO, 0.5, 1, changeColorTimer, start, middle);
			}
			//�������̐F
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
		//�X�e�[�W�̐F�ƃu���b�N�̐F����v���Ă����瓧��
		if (GameUtility::GetStageColor() == blockColor) {
			object.SetColor({ tmp.x, tmp.y, tmp.z, 0.5f });
		}
		else {
			object.SetColor({ tmp.x, tmp.y, tmp.z, 1 });
		}
	}

	//�O�t���[���̐F�ɍ��̐F�����Ă���
	prevStageColor = GameUtility::GetStageColor();
}