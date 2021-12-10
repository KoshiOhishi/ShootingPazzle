#include "HoleFloor.h"
#include "GameUtility.h"

void HoleFloor::Initialize(const StageVec2& pos)
{
	SetStagePos(pos);

	UpdateCollision();
}

void HoleFloor::Update()
{
	object.Update();
	UpdateCollision();
}

void HoleFloor::Draw()
{
}

void HoleFloor::UpdateCollision()
{
}