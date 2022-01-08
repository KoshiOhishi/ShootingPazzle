#include "NormalFloor.h"
#include "GameUtility.h"

ObjModel NormalFloor::modelBox;

void NormalFloor::CreateModel()
{
    //モデル生成
    //modelBox.CreateFromOBJ("NormalFloor");
    modelBox.CreateFromOBJ("NormalFloor");
}

void NormalFloor::StaticInitialize()
{
	CreateModel();
}

void NormalFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();

	objectName = "NormalFloor";
}

void NormalFloor::Update()
{
	object.Update(*pIOD);
	UpdateCollision();
}

void NormalFloor::UpdateCollision()
{
}