#include "NormalFloor.h"
#include "GameUtility.h"

ObjModel NormalFloor::modelBox;

void NormalFloor::StaticInitialize()
{
    //モデル生成
    //modelBox.CreateFromOBJ("NormalFloor");
    modelBox.CreateFromOBJ(MODEL_DIR + "NormalFloor/NormalFloor.obj");
}

void NormalFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
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
