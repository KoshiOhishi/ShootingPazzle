#include "BreakFloor.h"
#include "GameUtility.h"
#include "GameSound.h"

ObjModel BreakFloor::modelBox;

void BreakFloor::CreateModel()
{
	//���f������
	modelBox.CreateFromOBJ(modelDir + "BreakFloor/BreakFloor.obj");
}

void BreakFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();

	isBreak = false;

	objectName = "BreakFloor";
}

void BreakFloor::Update()
{
	//���������̏ォ�痣�ꂽ��j��
	{
		if (onFloor == false && prevOnFloor == true) {
			Break();
		}
		prevOnFloor = onFloor;
	}

	if (isBreak == false) {
		object.Update(*pIOD);
	}
	UpdateCollision();
}

void BreakFloor::UpdateCollision()
{
}

void BreakFloor::Break()
{
#ifdef BUILD_GAME

	//���ʉ��Đ�
	GameSound::Play(L"Break", object.GetPosition());

#endif // BUILD_GAME


	isBreak = true;
	//���Ƃ݂Ȃ�
	objectName = "HoleFloor";
}
