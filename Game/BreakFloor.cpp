#include "BreakFloor.h"
#include "GameUtility.h"

InstancingObjectDraw BreakFloor::instancingObjectDraw;
ObjModel BreakFloor::modelBox;

void BreakFloor::CreateModel()
{
	//���f������
	modelBox.CreateFromOBJ("BreakFloor");
}

void BreakFloor::StaticInitialize()
{
	instancingObjectDraw.Initialize();
	instancingObjectDraw.SetObjModel(&modelBox);
}

void BreakFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();
	object.SetColor({ 0.5f,0.5f,0.5f,1 });

	SetStagePos(pos);

	UpdateCollision();

	isBreak = false;
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
		object.Update(instancingObjectDraw);
	}
	UpdateCollision();
}

void BreakFloor::Draw()
{
	instancingObjectDraw.Update();
	instancingObjectDraw.Draw();
	instancingObjectDraw.EndDraw();
}

void BreakFloor::EndDraw()
{
	instancingObjectDraw.EndDraw();
}

void BreakFloor::UpdateCollision()
{
}

void BreakFloor::Break()
{
	isBreak = true;
}
