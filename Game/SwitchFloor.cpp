#include "SwitchFloor.h"
#include "GameUtility.h"

InstancingObjectDraw SwitchFloor::instancingObjectDraw;
ObjModel SwitchFloor::modelBox;

void SwitchFloor::CreateModel()
{
	//���f������
	modelBox.CreateFromOBJ("SwitchFloor");
}

void SwitchFloor::StaticInitialize()
{
	instancingObjectDraw.Initialize();
	instancingObjectDraw.SetObjModel(&modelBox);
}

void SwitchFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();
	object.SetColor({ 0.5f,0.5f,0.5f,1 });

	SetStagePos(pos);

	UpdateCollision();
}

void SwitchFloor::Update()
{
	object.Update(instancingObjectDraw);
	UpdateCollision();
}

void SwitchFloor::Draw()
{
	instancingObjectDraw.Update();
	instancingObjectDraw.Draw();
}

void SwitchFloor::UpdateCollision()
{
}