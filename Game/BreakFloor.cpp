#include "BreakFloor.h"
#include "GameUtility.h"

ObjModel BreakFloor::modelBox;

void BreakFloor::CreateModel()
{
	//���f������
	modelBox.CreateFromOBJ("BreakFloor");
}

void BreakFloor::StaticInitialize()
{
	CreateModel();
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
	isBreak = true;
	//���Ƃ݂Ȃ�
	objectName = "HoleFloor";
}
