#pragma once
#include "Object3D.h"
#include "GameUtility.h"

class StartLane
{
private:
	DX12Library::Object3D object;
	DX12Library::ObjModel model;
	StageVec2* stageSize;

public:
	void Initialize(StageVec2* stageSize);
	void Update();
	void Draw();

	void CreateModel();

	void SetPosition(unsigned short stagePosZ);
};

