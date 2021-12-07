#pragma once
#include "Object3D.h"
#include "GameUtility.h"

class StartLane
{
private:
	Object3D object;
	ObjModel model;
	StageVec2* stageSize;

public:
	void Initialize(StageVec2* stageSize);
	void Update();
	void Draw();

	void CreateModel();

	void SetPosition(unsigned short stagePosZ);
};

