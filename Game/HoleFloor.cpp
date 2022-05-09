#include "HoleFloor.h"
#include "GameUtility.h"

using namespace DX12Library;

void HoleFloor::Initialize(const StageVec2& pos)
{
	object.Initialize();
	SetStagePos(pos);

	UpdateCollision();
}

void HoleFloor::Update()
{
	UpdateCollision();
}

void HoleFloor::UpdateCollision()
{
}

int HoleFloor::GetFloorType()
{
	return FLOORTYPE_HOLE;
}
