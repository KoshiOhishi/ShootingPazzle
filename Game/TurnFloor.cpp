#include "TurnFloor.h"
#include "GameUtility.h"

InstancingObjectDraw TurnFloor::instancingObjectDraw[4];
ObjModel TurnFloor::modelBox[4];

void TurnFloor::CreateModel()
{
	//モデル生成
	modelBox[TURNTYPE_LEFT].CreateFromOBJ("TurnFloor_Left");
	modelBox[TURNTYPE_RIGHT].CreateFromOBJ("TurnFloor_Right");
	modelBox[TURNTYPE_UP].CreateFromOBJ("TurnFloor_Up");
	modelBox[TURNTYPE_DOWN].CreateFromOBJ("TurnFloor_Down");
}

void TurnFloor::StaticInitialize()
{
	for (int i = 0; i < 4; i++) {
		instancingObjectDraw[i].Initialize();
		instancingObjectDraw[i].SetObjModel(&modelBox[i]);
	}
}

void TurnFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();
}

void TurnFloor::Update()
{
	object.Update(instancingObjectDraw[turnType]);
	UpdateCollision();
}

void TurnFloor::Draw(int index)
{
	if (index == -1) {
		for (int i = 0; i < 4; i++) {
			instancingObjectDraw[i].Update();
			instancingObjectDraw[i].Draw();
		}
	}
	else {
		if (index < -1 || index >= 4) { return; }
		instancingObjectDraw[index].Update();
		instancingObjectDraw[index].Draw();
	}
}

void TurnFloor::EndDraw()
{
	for (int i = 0; i < 4; i++) {
		instancingObjectDraw[i].EndDraw();
	}
}

void TurnFloor::UpdateCollision()
{

}

void TurnFloor::SetTurnType(int turnType)
{
	this->turnType = turnType;
	//上と下の場合、オブジェクトを180度回転させる
	if (turnType == TURNTYPE_UP || turnType == TURNTYPE_DOWN) {
		object.SetRotation({0, 180, 0});
	}
}