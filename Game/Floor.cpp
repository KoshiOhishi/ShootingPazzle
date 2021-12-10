#include "Floor.h"
#include "GameUtility.h"

void Floor::CreateModel(const StageVec2& stageSize)
{
	size.x = stageSize.x * ONE_CELL_LENGTH;
	size.y = stageSize.y * ONE_CELL_LENGTH;

	model.CreateSquare(size.x, size.y, { 0.7f, 0.7f, 0.7f });
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
