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
	//�ŏ�����0�̎��͉��Ȃ��u���b�N
	isBreakable = breakupCount > 0;

	//�Ƃ肠��������u���b�N�͐F�ς��Ƃ�
	if (breakupCount == 2) {
		//��
		object.SetColor(0.5f, 1, 0.5f, 1);
	}
	else if (breakupCount == 1) {
		//��
		object.SetColor(1, 0.5f, 0.5f, 1);
	}
	else if (breakupCount == 0) {
		//��
		object.SetColor(1, 1, 1, 1);
	}
}