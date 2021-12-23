#include "HoleFloor.h"
#include "GameUtility.h"

void HoleFloor::StaticInitialize()
{
}

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

void HoleFloor::Draw()
{
}

void HoleFloor::EndDraw()
{
}

void HoleFloor::UpdateCollision()
{
}