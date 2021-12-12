#include "TurnFloor.h"
#include "GameUtility.h"

ObjModel TurnFloor::modelBox[4];

void TurnFloor::CreateModel()
{
	//���f������
	modelBox[TURNTYPE_LEFT].CreateFromOBJ("TurnFloor_Left");
	modelBox[TURNTYPE_RIGHT].CreateFromOBJ("TurnFloor_Right");
	modelBox[TURNTYPE_UP].CreateFromOBJ("TurnFloor_Up");
	modelBox[TURNTYPE_DOWN].CreateFromOBJ("TurnFloor_Down");
}

void TurnFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();

	//���������Ȃ��̂Ń��f�����܂ŐF�ς���
	object.SetColor(0.5f, 0.5f, 0.5f, 1);

	SetStagePos(pos);

	UpdateCollision();
}

void TurnFloor::Update()
{
	object.Update();
	UpdateCollision();
}

void TurnFloor::Draw()
{
	object.Draw();
}

void TurnFloor::UpdateCollision()
{

}

void TurnFloor::SetTurnType(int turnType)
{
	this->turnType = turnType;
	object.SetObjModel(&modelBox[turnType]);
}