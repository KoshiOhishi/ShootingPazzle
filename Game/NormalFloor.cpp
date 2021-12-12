#include "NormalFloor.h"
#include "GameUtility.h"

ObjModel NormalFloor::modelBox;

void NormalFloor::CreateModel()
{
    //モデル生成
    modelBox.CreateFromOBJ("NormalFloor");
}

void NormalFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();
	object.SetObjModel(&modelBox);

	//見分けつかないのでモデル作るまで色変える
	object.SetColor(0.5f, 0.5f, 0.5f, 1);

	SetStagePos(pos);

	UpdateCollision();
}

void NormalFloor::Update()
{
	object.Update();
	UpdateCollision();
}

void NormalFloor::Draw()
{
	object.Draw();
}

void NormalFloor::UpdateCollision()
{
}