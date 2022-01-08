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

void TurnFloor::StaticInitialize()
{
	CreateModel();
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
	object.Update(pIOD[turnType]);
	UpdateCollision();
}

void TurnFloor::UpdateCollision()
{

}

void TurnFloor::SetTurnType(int turnType)
{
	this->turnType = turnType;

	switch (turnType) {
	case TURNTYPE_LEFT:		objectName = "TurnFloor_Left";		break;
	case TURNTYPE_RIGHT:	objectName = "TurnFloor_Right";		break;
	case TURNTYPE_UP:		objectName = "TurnFloor_Up";
							object.SetRotation({ 0,180,0 });	break;
	case TURNTYPE_DOWN:		objectName = "TurnFloor_Down";
							object.SetRotation({ 0,180,0 });	break;
	}
}