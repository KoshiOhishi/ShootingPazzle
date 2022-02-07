#include "NormalFloor.h"
#include "GameUtility.h"

ObjModel NormalFloor::modelBox;

void NormalFloor::StaticInitialize()
{
    //���f������
    //modelBox.CreateFromOBJ("NormalFloor");
    modelBox.CreateFromOBJ(MODEL_DIR + "NormalFloor/NormalFloor.obj");
}

void NormalFloor::Initialize(const StageVec2& pos)
{
	//�I�u�W�F�N�g����
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();
}

void NormalFloor::Update()
{
	object.Update(*pIOD);
	UpdateCollision();
}

void NormalFloor::UpdateCollision()
{
}

int NormalFloor::GetFloorType()
{
	return FLOORTYPE_NORMAL;
}
