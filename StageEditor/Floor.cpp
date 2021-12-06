#include "Floor.h"

void Floor::CreateModel(float width, float depth)
{
	model.CreateSquare(width, depth, { 0.7f, 0.7f, 0.7f });
}

void Floor::Initialize()
{
	Object3D::Initialize();
	Object3D::SetObjModel(&model);
	//°‚È‚Ì‚Å90“x‰ñ“]
	Object3D::SetRotation(90, 0, 0);

	//“–‚½‚è”»’è‰Šú‰»
	plane.distance = 0;
	plane.normal = { 0,1,0 };	//–@ü•ûŒü‚Íã
}
