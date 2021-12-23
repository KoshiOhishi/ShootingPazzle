#include "BreakFloor.h"
#include "GameUtility.h"

InstancingObjectDraw BreakFloor::instancingObjectDraw;
ObjModel BreakFloor::modelBox;

void BreakFloor::CreateModel()
{
	//モデル生成
	modelBox.CreateFromOBJ("BreakFloor");
}

void BreakFloor::StaticInitialize()
{
	instancingObjectDraw.Initialize();
	instancingObjectDraw.SetObjModel(&modelBox);
}

void BreakFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();
	object.SetColor({ 0.5f,0.5f,0.5f,1 });

	SetStagePos(pos);

	UpdateCollision();

	isBreak = false;
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
		object.Update(instancingObjectDraw);
	}
	UpdateCollision();
}

void BreakFloor::Draw()
{
	instancingObjectDraw.Update();
	instancingObjectDraw.Draw();
	instancingObjectDraw.EndDraw();
}

void BreakFloor::EndDraw()
{
	instancingObjectDraw.EndDraw();
}

void BreakFloor::UpdateCollision()
{
}

void BreakFloor::Break()
{
	isBreak = true;
}
