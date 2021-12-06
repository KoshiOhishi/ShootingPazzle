#pragma once
#include "Object3D.h"
#include "GameUtility.h"

class StartLane
{
private:
	Object3D object;
	ObjModel model;
	StageVec2 stageSize;

public:
	void Initialize(const StageVec2& stageSize);
	void Update();
	void Draw();

	void SetPosition(unsigned short stagePosZ);
};

