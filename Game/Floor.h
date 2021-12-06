#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"

class Floor : public Object3D
{
private:
	ObjModel model;
	Plane plane;

public:
	void CreateModel(float width, float depth);

	void Initialize() override;

	const Plane& GetPlane()const { return plane; }
};

