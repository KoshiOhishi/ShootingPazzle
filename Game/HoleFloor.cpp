#include "HoleFloor.h"
#include "GameUtility.h"

void HoleFloor::Initialize(const StageVec2& pos)
{
	object.Initialize();
	SetStagePos(pos);

	UpdateCollision();

	objectName = "HoleFloor";
}

void HoleFloor::Update()
{
	UpdateCollision();
}

void HoleFloor::UpdateCollision()
{
}