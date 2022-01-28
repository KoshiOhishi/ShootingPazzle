#include "TurnFloor.h"
#include "GameUtility.h"

ObjModel TurnFloor::modelBox[4];
ObjModel TurnFloor::modelCover[4];

void TurnFloor::CreateModel()
{
	//モデル生成
	modelBox[TURNTYPE_LEFT].CreateFromOBJ(modelDir + "TurnFloor_Left/TurnFloor_Left.obj");
	modelBox[TURNTYPE_RIGHT].CreateFromOBJ(modelDir + "TurnFloor_Right/TurnFloor_Right.obj");
	modelBox[TURNTYPE_UP].CreateFromOBJ(modelDir + "TurnFloor_Up/TurnFloor_Up.obj");
	modelBox[TURNTYPE_DOWN].CreateFromOBJ(modelDir + "TurnFloor_Down/TurnFloor_Down.obj");

	modelCover[TURNTYPE_LEFT].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, modelDir + "TurnFloor_Left/uv_cover.png");
	modelCover[TURNTYPE_RIGHT].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, modelDir + "TurnFloor_Right/uv_cover.png");
	modelCover[TURNTYPE_UP].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, modelDir + "TurnFloor_Up/uv_cover.png");
	modelCover[TURNTYPE_DOWN].CreatePlaneTex(ONE_CELL_LENGTH, ONE_CELL_LENGTH, modelDir + "TurnFloor_Down/uv_cover.png");
}

void TurnFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	//カバーオブジェクト位置セット
	objCover.Initialize();
	objCover.SetPosition(object.GetPosition() + Vector3(0, ONE_CELL_LENGTH / 2, 0));
	objCover.SetRotation({ 90, 0, 0 });

	UpdateCollision();
}

void TurnFloor::Update()
{
	objCover.SetPosition(object.GetPosition() + Vector3(0, ONE_CELL_LENGTH / 2, 0));
	RollUV();
	object.Update(pIOD[turnType]);
	objCover.Update(pIOD[turnType + 4]);
	UpdateCollision();
}

void TurnFloor::UpdateCollision()
{

}

void TurnFloor::SetTurnType(int turnType)
{
	this->turnType = turnType;

	switch (turnType) {
	case TURNTYPE_LEFT:		objectName = "TurnFloor_Left";		break;
	case TURNTYPE_RIGHT:	objectName = "TurnFloor_Right";		break;
	case TURNTYPE_UP:		objectName = "TurnFloor_Up";
							object.SetRotation({ 0,180,0 });	break;
	case TURNTYPE_DOWN:		objectName = "TurnFloor_Down";
							object.SetRotation({ 0,180,0 });	break;
	}
}

void TurnFloor::RollUV()
{
	const Vector2 OLD_UV[4] = {
		{0,1}, {0,0}, {1,1}, {1,0} 
	};
	static float add = 0;
	add += 0.001f;
	if (add >= 1) { add -= 1; }

	for (int i = 0; i < modelCover[i].GetVertices().size(); i++) {
		modelCover[TURNTYPE_LEFT].SetVertexUV(i, OLD_UV[i] + Vector2(add, 0));
		modelCover[TURNTYPE_RIGHT].SetVertexUV(i, OLD_UV[i] + Vector2(-add, 0));
		modelCover[TURNTYPE_UP].SetVertexUV(i, OLD_UV[i] + Vector2(0, add));
		modelCover[TURNTYPE_DOWN].SetVertexUV(i, OLD_UV[i] + Vector2(0, -add));
	}
}
