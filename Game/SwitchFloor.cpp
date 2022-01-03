#include "SwitchFloor.h"
#include "GameUtility.h"

InstancingObjectDraw SwitchFloor::instancingObjectDraw[2];
ObjModel SwitchFloor::modelBox[2];

void SwitchFloor::CreateModel()
{
	//モデル生成
	modelBox[SWITCH_STATE_OFF].CreateFromOBJ("SwitchFloor_OFF");
	modelBox[SWITCH_STATE_ON].CreateFromOBJ("SwitchFloor_ON");
}

void SwitchFloor::StaticInitialize()
{
	CreateModel();
	for (int i = 0; i < _countof(modelBox); i++) {
		instancingObjectDraw[i].Initialize();
		instancingObjectDraw[i].SetObjModel(&modelBox[i]);
	}
}

void SwitchFloor::Initialize(const StageVec2& pos)
{
	//オブジェクト生成
	object.Initialize();

	SetStagePos(pos);

	UpdateCollision();

	colorTimer.SetTimer(0, 1000, true, 0.5f);
	colorTimer.Start();
}

void SwitchFloor::Update()
{
	UpdateSwitchState();
	UpdateSwitchColor();
	object.Update(instancingObjectDraw[switchState]);
	UpdateCollision();
}

void SwitchFloor::Draw()
{
	for (int i = 0; i < _countof(modelBox); i++) {
		instancingObjectDraw[i].Update();
		instancingObjectDraw[i].Draw();
	}
}

void SwitchFloor::EndDraw()
{
	for (int i = 0; i < _countof(modelBox); i++) {
		instancingObjectDraw[i].EndDraw();
	}
}

void SwitchFloor::UpdateCollision()
{
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
	colorTimer.Update();

	if (switchState == SWITCH_STATE_OFF) {
		float digrees = colorTimer.GetNowTime() * 180 / colorTimer.GetEndTime();
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
	//オブジェクトのカラーセット
	switch (switchColor) {
	case SWITCH_COLOR_NONE:		objectName = "SwitchFloor_None"; break;
	case SWITCH_COLOR_RED:		objectName = "SwitchFloor_Red"; break;
	case SWITCH_COLOR_BLUE:		objectName = "SwitchFloor_Blue"; break;
	case SWITCH_COLOR_YELLOW:	objectName = "SwitchFloor_Yellow"; break;
	case SWITCH_COLOR_GREEN:	objectName = "SwitchFloor_Green"; break;
	}
}
