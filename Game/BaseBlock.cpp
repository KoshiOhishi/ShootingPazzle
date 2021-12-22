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
	if (effectEndTime == -1) {
		effectEndTime = 1700 + rand() % 200;
	}
	
	double y = Easing::GetEaseValue(firstEffectType, 300, ONE_CELL_LENGTH / 2, timer, 500, effectEndTime);

	Vector3 nPos = object.GetPosition();
	object.SetPosition(nPos.x, y, nPos.z);
}
