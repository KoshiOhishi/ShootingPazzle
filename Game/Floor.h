#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"

class Floor : public Object3D
{
private:
	Vector2 size;
	ObjModel model;
	Plane plane;

public:
	void CreateModel(const StageVec2& stageSize);

	void Initialize() override;

	const Plane& GetPlane()const { return plane; }

	const Vector2& GetFloorSize()const { return size; }
	const Vector3& GetFloorPos()const { return position; }
};

