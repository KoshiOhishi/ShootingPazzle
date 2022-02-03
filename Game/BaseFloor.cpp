#include "BaseFloor.h"
#include "GameUtility.h"
#include "Easing.h"

Stage* BaseFloor::pStage = nullptr;

void BaseFloor::UpdateFirstEffect(const Timer& timer)
{
	//���񂾂��G�t�F�N�g�̎�ތ���
	if (firstEffectType == -1) {
		//easeOut�n�̒����烉���_���ŁB(2,5,8,11,...)
		firstEffectType = rand() % 7;
		firstEffectType = 3 * firstEffectType + 2;
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
	//���񂾂��G�t�F�N�g�̃X�^�[�g���Ԍ���
	if (clearEffectStartTime == -1) {
		clearEffectStartTime = 500 + rand() % 1500;
	}

	//�^�C�}�[�̒l��clearEffectStartTime�𒴂��Ă�����u���b�N�𗎂Ƃ�
	if ((double)timer.GetNowTime() >= clearEffectStartTime && timer.GetIsStart()) {
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
