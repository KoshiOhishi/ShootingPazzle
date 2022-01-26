#include "StartLane.h"

void StartLane::Initialize(StageVec2* stageSize)
{
	this->stageSize = stageSize;
	CreateModel();
	object.Initialize();
	object.SetObjModel(&model);
	object.SetRotation(90,0,0);
	object.SetColor(1,0,0,0.6f);
}

void StartLane::Update()
{
	object.Update();
}

void StartLane::Draw()
{
	object.Draw();
}

void StartLane::CreateModel()
{
	model.CreatePlane(stageSize->x * ONE_CELL_LENGTH, ONE_CELL_LENGTH, { 1.0f,1.0f,1.0f });
}

void StartLane::SetPosition(unsigned short stagePosZ)
{
	float z;
	GameUtility::CalcStagePos2WorldPos({0,stagePosZ}, nullptr, &z);
	object.SetPosition(0, 0.1f, z);
}
