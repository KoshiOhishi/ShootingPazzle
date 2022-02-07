#include "SwitchFloor.h"
#include "GameUtility.h"
#include "FPSManager.h"

ObjModel SwitchFloor::modelBox[2];

void SwitchFloor::StaticInitialize()
{
	//モデル生成
	modelBox[SWITCH_STATE_OFF].CreateFromOBJ(MODEL_DIR + "SwitchFloor_OFF/SwitchFloor_OFF.obj");
	modelBox[SWITCH_STATE_ON].CreateFromOBJ(MODEL_DIR + "SwitchFloor_ON/SwitchFloor_ON.obj");
}

void SwitchFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();
}

void SwitchFloor::Update()
{
	UpdateSwitchState();
	UpdateSwitchColor();
	object.Update(pIOD[switchState]);
	UpdateCollision();
}

void SwitchFloor::UpdateCollision()
{
}

int SwitchFloor::GetFloorType()
{
	int result = -1;
	switch (switchColor) {
	case SWITCH_COLOR_NONE:
		result = FLOORTYPE_SWITCH_NONE;
		break;
	case SWITCH_COLOR_RED:
		result = FLOORTYPE_SWITCH_RED;
		break;
	case SWITCH_COLOR_BLUE:
		result = FLOORTYPE_SWITCH_BLUE;
		break;
	case SWITCH_COLOR_YELLOW:
		result = FLOORTYPE_SWITCH_YELLOW;
		break;
	case SWITCH_COLOR_GREEN:
		result = FLOORTYPE_SWITCH_GREEN;
		break;
	}

	return result;
}

void SwitchFloor::UpdateSwitchState()
{
	//自分の色とステージの色が一致していたらスイッチON状態
	if (GameUtility::GetStageColor() == switchColor) {
		switchState = SWITCH_STATE_ON;
	}
	else {
		switchState = SWITCH_STATE_OFF;
	}
}

void SwitchFloor::UpdateSwitchColor()
{
	addColorTime += 0.5 * FPSManager::GetMulAdjust60FPS();
	if (addColorTime >= 60) { addColorTime -= 60; }

	if (switchState == SWITCH_STATE_OFF) {
		float digrees = addColorTime * 180 / 60;
		float val = sin(digrees * PI / 180) * 0.75f;

		Vector4 add = { val,val,val,0 };

		switch (switchColor) {
		case SWITCH_COLOR_NONE:		object.SetColor(Vector4{ 1,	1, 1, 1 } + add); break;
		case SWITCH_COLOR_RED:		object.SetColor(Vector4{ 1,	0, 0, 1 } + add); break;
		case SWITCH_COLOR_BLUE:		object.SetColor(Vector4{ 0,	0, 1, 1 } + add); break;
		case SWITCH_COLOR_YELLOW:	object.SetColor(Vector4{ 1,	1, 0, 1 } + add); break;
		case SWITCH_COLOR_GREEN:	object.SetColor(Vector4{ 0,	1, 0, 1 } + add); break;
		}
	}
	else {
		switch (switchColor) {
		case SWITCH_COLOR_NONE:		object.SetColor({ 0.75f,0.75f,	0.75f,	1 }); break;
		case SWITCH_COLOR_RED:		object.SetColor({ 0.75f,0,		0,		1 }); break;
		case SWITCH_COLOR_BLUE:		object.SetColor({ 0,	0,		0.75f,	1 }); break;
		case SWITCH_COLOR_YELLOW:	object.SetColor({ 0.75f,0.75f,	0,		1 }); break;
		case SWITCH_COLOR_GREEN:	object.SetColor({ 0,	0.75f,	0,		1 }); break;
		}
	}
}

void SwitchFloor::SetSwitchColor(int switchColor)
{
	this->switchColor = switchColor;
}
