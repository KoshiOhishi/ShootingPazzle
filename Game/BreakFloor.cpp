#include "BreakFloor.h"
#include "GameUtility.h"

ObjModel BreakFloor::modelBox;

void BreakFloor::CreateModel()
{
	//モデル生成
	modelBox.CreateFromOBJ("BreakFloor");
}

void BreakFloor::StaticInitialize()
{
	CreateModel();
}

void BreakFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();

	isBreak = false;

	objectName = "BreakFloor";
}

void BreakFloor::Update()
{
	//球が自分の上から離れたら破壊
	{
		if (onFloor == false && prevOnFloor == true) {
			Break();
		}
		prevOnFloor = onFloor;
	}

	if (isBreak == false) {
		object.Update(*pIOD);
	}
	UpdateCollision();
}

void BreakFloor::UpdateCollision()
{
}

void BreakFloor::Break()
{
	isBreak = true;
	//穴とみなす
	objectName = "HoleFloor";
}
