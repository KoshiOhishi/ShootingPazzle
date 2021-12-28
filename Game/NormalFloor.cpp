#include "NormalFloor.h"
#include "GameUtility.h"

InstancingObjectDraw NormalFloor::instancingObjectDraw;
ObjModel NormalFloor::modelBox;

void NormalFloor::CreateModel()
{
    //���f������
    //modelBox.CreateFromOBJ("NormalFloor");
    modelBox.CreateFromOBJ("NormalFloor");
}

void NormalFloor::StaticInitialize()
{
	CreateModel();
	instancingObjectDraw.Initialize();
	instancingObjectDraw.SetObjModel(&modelBox);
}

void NormalFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();

	objectName = "NormalFloor";
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