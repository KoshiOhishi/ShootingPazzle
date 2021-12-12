#include "TurnFloor.h"
#include "GameUtility.h"

ObjModel TurnFloor::modelBox[4];

void TurnFloor::CreateModel()
{
	//モデル生成
	modelBox[TURNTYPE_LEFT].CreateFromOBJ("TurnFloor_Left");
	modelBox[TURNTYPE_RIGHT].CreateFromOBJ("TurnFloor_Right");
	modelBox[TURNTYPE_UP].CreateFromOBJ("TurnFloor_Up");
	modelBox[TURNTYPE_DOWN].CreateFromOBJ("TurnFloor_Down");
}

void TurnFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	//見分けつかないのでモデル作るまで色変える
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