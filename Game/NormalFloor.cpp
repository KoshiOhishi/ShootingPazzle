#include "NormalFloor.h"
#include "GameUtility.h"

InstancingObjectDraw NormalFloor::instancingObjectDraw;
ObjModel NormalFloor::modelBox;

void NormalFloor::CreateModel()
{
    //モデル生成
    //modelBox.CreateFromOBJ("NormalFloor");
    modelBox.CreateFromOBJ("NormalFloor");
}

void NormalFloor::StaticInitialize()
{
	instancingObjectDraw.Initialize();
	instancingObjectDraw.SetObjModel(&modelBox);
}

void NormalFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();
	object.SetColor({ 0.5f,0.5f,0.5f,1 });

	SetStagePos(pos);

	UpdateCollision();
}

void NormalFloor::Update()
{
	object.Update(instancingObjectDraw);
	UpdateCollision();
}

void NormalFloor::Draw()
{
	instancingObjectDraw.Update();
	instancingObjectDraw.Draw();
}

void NormalFloor::EndDraw()
{
	instancingObjectDraw.EndDraw();
}

void NormalFloor::UpdateCollision()
{
}