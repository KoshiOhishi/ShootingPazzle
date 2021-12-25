#include "BaseBlock.h"
#include "GameUtility.h"
#include "Easing.h"

Stage* BaseBlock::pStage = nullptr;

void BaseBlock::SetStagePos(const StageVec2& pos)
{
	float x, z;
	GameUtility::CalcStagePos2WorldPos(pos, &x, &z);

	object.SetPosition(x, ONE_CELL_LENGTH / 2, z);
}

void BaseBlock::SetBreakupCount(unsigned short breakupCount)
{
	this->breakupCount = breakupCount;
	//�ŏ�����0�̎��͉��Ȃ��u���b�N
	isBreakable = breakupCount > 0;
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
	object.SetPosition(nPos.x, y, nPos.z);
}

void BaseBlock::UpdateClearEffect(const Timer& timer)
{
	//���񂾂��G�t�F�N�g�̃X�^�[�g���Ԍ���
	if (clearEffectStartTime == -1) {
		clearEffectStartTime = 1000 + rand() % 1500;
	}

	//�^�C�}�[�̒l��clearEffectStartTime�𒴂��Ă�����u���b�N�𗎂Ƃ�
	if ((double)timer.GetNowTime() >= clearEffectStartTime) {
		Vector3 nPos = object.GetPosition();
		float sub = (timer.GetNowTime() - clearEffectStartTime) * 0.01f;
		object.SetPosition({ nPos.x, nPos.y - sub, nPos.z });
	}
}
