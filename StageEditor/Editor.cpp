#include <fstream>
#include "Editor.h"
#include "DX12Util.h"
#include "Input.h"
#include "Sound.h"
#include "DebugText.h"
#include "Collision.h"
#include "FPSManager.h"
#include "FbxLoader.h"
#include "ImguiHelper.h"
#include "GameUtility.h"
#include "NormalFloor.h"
#include "TurnFloor.h"
#include "BreakFloor.h"
#include "SwitchFloor.h"

#include "../DX12Library/imgui/imgui.h"
#include "../DX12Library/imgui/imgui_impl_win32.h"
#include "../DX12Library/imgui/imgui_impl_dx12.h"

Editor::Editor()
{
	modelBG.CreateFromOBJ(MODEL_DIR + "Sky/Sky.obj");
}

void Editor::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 0.1f, 150.0f);

	//�J����������
	camera.Initialize();
	//camera.SetPositionAndDistance({ 0,100.0f,0.0f }, 30.0f);
	camera.SetEyeTargetUpVec({ 0,100,0 }, { 0,0,0 }, { 0,0,1 });
	camera.SetRotation(90, 0, 0);

	//�J�������Z�b�g
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//���C�g������
	light.Initialize();
	light.SetLightDir({ 0.5,-1,0.5 });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(80.0f);
	Object3D::SetLight(&light);

	//�t�F�[�Y������
	GameUtility::SetNowPhase(PHASE_GAME_SETPOS);

	//�w�i�I�u�W�F�N�g������
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	//�^�C�}�[�Z�b�g
	timer.SetTimer(0, 1000000);
	timer.Start();

	//�X�e�[�W�擾
	stage.Initialize();
	stage.LoadStage("");
	sliderWidth = stage.stageSize.x;
	sliderDepth = stage.stageSize.y;
	prevSize = stage.stageSize;

	//�I�u�W�F�N�g������
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].Initialize(&stage.stageSize);
	}
#pragma region �\���p�I�u�W�F�N�g������
	objDispBlock.Initialize();
	objDispBlock.SetObjModel(SquareBlock::GetModel(0));
	objDispFloor.Initialize();
	objDispFloor.SetObjModel(NormalFloor::GetModel());
	objDispFloorSub.Initialize();
	objDispFloorSub.SetObjModel(TurnFloor::GetModel(TURNTYPE_LEFT + 4));
	objDispFloorSub.SetRotation({ 90,0,0 });
#pragma endregion

	//�e�L�X�g�`�揉����
	FontData fd = {};
	fd.fontName = L"HGP�޼��E";
	fd.height = 32;
	txtCursol.SetFontData(fd);

	//�R�}���h������
	Command::Initialize();
	Command::SetPStage(&stage);
}

void Editor::Update()
{
	//�J�����X�V
	UpdateCamera();

	light.Update();
	timer.Update();
	//3D�T�E���h�Ŏg�p���郊�X�i�[�̈ʒu�X�V
	Sound::Set3DListenerPosAndVec(camera);

	//�w�i�I�u�W�F�N�g�X�V
	objBG.AddRotation(0, 0.1f * FPSManager::GetMulAdjust60FPS(), 0);
	objBG.Update();

	//�G�f�B�^�[�����̍X�V����
	UpdateEdit();

}

void Editor::Draw()
{
	//�w�i�`��
	objBG.Draw();

	//�X�e�[�W�`��
	stage.Draw();

	//�X�^�[�g���[���`��
	DrawStartLane();

	if (mode == MODE_ADD) {
		//�J�[�\���ʒu�Ƀu���b�N�`��
		if (objectType == OBJECTTYPE_BLOCK) {
			DrawDispBlock();
		}
		else if (objectType == OBJECTTYPE_FLOOR) {
			DrawDispFloor();
		}
	}
	else if (mode == MODE_DELETE) {

	}

	//���̃J�[�\�����W�\��
	if (IsInsideCursol()) {
		txtCursol.DrawStringFG(0, 0, L"StagePos:(" + std::to_wstring(nowCursolPos.x) + L", " + std::to_wstring(nowCursolPos.y) + L")");
	}
}

void Editor::UpdateCamera()
{
	if (isEnteringIOName == false) {
		//�J�����ړ�
		if (Keyboard::IsKeyPush(DIK_W)) {
			camera.MoveCamera(0, 0, 1, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_S)) {
			camera.MoveCamera(0, 0, -1, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_A)) {
			camera.MoveCamera(-1, 0, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_D)) {
			camera.MoveCamera(1, 0, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_E)) {
			camera.MoveCamera(0, 1, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_X)) {
			camera.MoveCamera(0, -1, 0, false, true, false);
		}
		//Vector3 f = { camera.GetRotation().x, camera.GetRotation().y - 1.5f, camera.GetRotation().z };
		//camera.SetRotation(f);
	}

	camera.Update();
}

void Editor::UpdateEdit()
{
	UpdateImgui();
	//���݂̃}�E�X�J�[�\���̈ʒu���X�e�[�W���W��Ōv�Z
	CalcNowCursolPos();
	//�\���p�I�u�W�F�N�g�X�V
	UpdateObject();
	//�X�e�[�W�X�V
	UpdateStage();

	//���[�h���Ƃ̍X�V
	if (mode == MODE_ADD) {
		UpdateAdd();
	}
	else if (mode == MODE_DELETE) {
		UpdateDelete();
	}
	else if (mode == MODE_OPTION) {
		UpdateOption();
	}
	Command::Update();
}

void Editor::UpdateAdd()
{
	//�X�e�[�W�͈͊O�Ȃ瑦���^�[��
	if (IsInsideCursol() == false) {
		return;
	}

	if (objectType == OBJECTTYPE_BLOCK) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			//�X�e�[�W�ɒǉ�
			stage.AddBlock(nowCursolPos, blockType, breakupCount, blockColor);
		}
	}
	else if (objectType == OBJECTTYPE_FLOOR) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			//�X�e�[�W�ɒǉ�
			stage.AddFloor(nowCursolPos, floorType);
		}
	}
}

void Editor::UpdateDelete()
{
	//�X�e�[�W�͈͊O�Ȃ瑦���^�[��
	if (IsInsideCursol() == false) {
		return;
	}

	if (objectType == OBJECTTYPE_BLOCK) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			//�X�e�[�W����폜
			stage.DeleteBlock(nowCursolPos);
		}
	}
	else if (objectType == OBJECTTYPE_FLOOR) {
		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			//�X�e�[�W����폜
			stage.DeleteFloor(nowCursolPos);
		}
	}
}

void Editor::UpdateOption()
{
	//�傫�����ύX���ꂽ�Ƃ��̏���
	if (sliderWidth != stage.stageSize.x) {
		//�R�}���h�ɒǉ�
		CommandDetail data;
		data.commandType = COMMAND_TYPE_STAGE_WIDTH;
		data.args1 = stage.stageSize.x;
		data.args2 = sliderWidth;
		Command::AddCommand(data);
	}
	else if (sliderDepth != stage.stageSize.y) {
		//�R�}���h�ɒǉ�
		CommandDetail data;
		data.commandType = COMMAND_TYPE_STAGE_HEIGHT;
		data.args1 = stage.stageSize.y;
		data.args2 = sliderDepth;
		Command::AddCommand(data);
	}

	//���f
	stage.stageSize.x = sliderWidth;
	stage.stageSize.y = sliderDepth;
}

void Editor::UpdateObject()
{
	UpdateDispObject();

	//�u���b�N�ʒu���J�[�\���ʒu��
	float x, z;
	GameUtility::CalcStagePos2WorldPos(nowCursolPos, &x, &z);
	objDispBlock.SetPosition({ x, ONE_CELL_LENGTH / 2, z });
	objDispFloor.SetPosition({ x, -ONE_CELL_LENGTH / 2, z });
	objDispFloorSub.SetPosition({ x, 0, z });
	//�X�V
	UpdateStartLane();
	objDispBlock.Update();
	objDispFloor.Update();
	objDispFloorSub.Update();
	
}

void Editor::UpdateStage()
{
	//�O�t���[������X�e�[�W�̑傫�����ς���Ă�����X�e�[�W���č\�z
	if (prevSize.x != stage.stageSize.x || prevSize.y != stage.stageSize.y) {
		StageVec2 nowSize = stage.stageSize;
		ReCreateStage(prevSize, nowSize);
		//Imgui�̃X���C�_�[�̒l���X�V���Ă���
		sliderWidth = nowSize.x;
		sliderDepth = nowSize.y;
	}

	prevSize = stage.stageSize;

	//�X�e�[�W�X�V
	stage.Update();
}

void Editor::UpdateDispObject()
{
	//���f�����Z�b�g����
	//�u���b�N
	objDispBlock.SetColor(0.5f, 0.5f, 0.5f, 1);
	objDispBlock.SetRotation({ 0, 0, 0 });

	switch (blockType) {
	case BLOCKTYPE_SQUARE:					objDispBlock.SetObjModel(SquareBlock::GetModel(breakupCount)); break;
	case BLOCKTYPE_TRIANGLE_NO_LEFTTOP:		objDispBlock.SetObjModel(TriangleBlock::GetModel(breakupCount));
											objDispBlock.SetRotation({ 0, 180, 0 }); break;
	case BLOCKTYPE_TRIANGLE_NO_RIGHTTOP:	objDispBlock.SetObjModel(TriangleBlock::GetModel(breakupCount));
											objDispBlock.SetRotation({ 0, 270, 0 }); break;
	case BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM:	objDispBlock.SetObjModel(TriangleBlock::GetModel(breakupCount));
											objDispBlock.SetRotation({ 0, 90, 0 }); break;
	case BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM: objDispBlock.SetObjModel(TriangleBlock::GetModel(breakupCount)); break;
	}

	//�F�Z�b�g
	if (breakupCount > 0) {
		objDispBlock.SetColor(GameUtility::COLOR_VALUE_BREAKABLE[blockColor]);
	}
	else {
		objDispBlock.SetColor(GameUtility::COLOR_VALUE[blockColor]);
	}

	//��
	objDispFloor.SetColor(1, 1, 1, 1);
	objDispFloor.SetRotation({ 0, 0, 0 });
	switch (floorType) {
	case FLOORTYPE_NORMAL:			objDispFloor.SetObjModel(NormalFloor::GetModel());
									objDispFloorSub.SetObjModel(nullptr); break;
	case FLOORTYPE_TURN_LEFT:		objDispFloor.SetObjModel(TurnFloor::GetModel(TURNTYPE_LEFT)); 
									objDispFloorSub.SetObjModel(TurnFloor::GetModel(TURNTYPE_LEFT + 4)); break;
	case FLOORTYPE_TURN_RIGHT:		objDispFloor.SetObjModel(TurnFloor::GetModel(TURNTYPE_RIGHT));
									objDispFloorSub.SetObjModel(TurnFloor::GetModel(TURNTYPE_RIGHT + 4)); break;
	case FLOORTYPE_TURN_UP:			objDispFloor.SetObjModel(TurnFloor::GetModel(TURNTYPE_UP));
									objDispFloor.SetRotation({ 0, 180, 0 });
									objDispFloorSub.SetObjModel(TurnFloor::GetModel(TURNTYPE_UP + 4)); break;
	case FLOORTYPE_TURN_DOWN:		objDispFloor.SetObjModel(TurnFloor::GetModel(TURNTYPE_DOWN));
									objDispFloor.SetRotation({ 0, 180, 0 });
									objDispFloorSub.SetObjModel(TurnFloor::GetModel(TURNTYPE_DOWN + 4)); break;
	case FLOORTYPE_BREAK:			objDispFloor.SetObjModel(BreakFloor::GetModel());
									objDispFloorSub.SetObjModel(nullptr); break;
	case FLOORTYPE_SWITCH_NONE:		objDispFloor.SetObjModel(SwitchFloor::GetModel(SWITCH_STATE_ON));
									objDispFloorSub.SetObjModel(nullptr); break;
	case FLOORTYPE_SWITCH_RED:		objDispFloor.SetObjModel(SwitchFloor::GetModel(SWITCH_STATE_OFF));
									objDispFloor.SetColor(1, 0.66f, 0.66f, 1);
									objDispFloorSub.SetObjModel(nullptr); break;
	case FLOORTYPE_SWITCH_BLUE:		objDispFloor.SetObjModel(SwitchFloor::GetModel(SWITCH_STATE_OFF));
									objDispFloor.SetColor(0, 0, 1, 1);
									objDispFloorSub.SetObjModel(nullptr); break;
	case FLOORTYPE_SWITCH_YELLOW:	objDispFloor.SetObjModel(SwitchFloor::GetModel(SWITCH_STATE_OFF));
									objDispFloor.SetColor(1, 1, 0, 1);
									objDispFloorSub.SetObjModel(nullptr); break;
	case FLOORTYPE_SWITCH_GREEN:	objDispFloor.SetObjModel(SwitchFloor::GetModel(SWITCH_STATE_OFF));
									objDispFloor.SetColor(0, 1, 0, 1);
									objDispFloorSub.SetObjModel(nullptr); break;
	}
}

void Editor::UpdateStartLane()
{
	//0...�ݒu���ꂽ�X�^�[�g���[��
	startLane[0].SetPosition(stage.startLaneZ);
	startLane[0].Update();
	//1...�J�[�\���ʒu�ɕ`�悳���X�^�[�g���[��
	if (mode == MODE_OPTION && optionMode == OPTION_SET_STARTLANE && IsInsideCursol()) {

		startLane[1].SetPosition(nowCursolPos.y);

		//�J�[�\���̈ʒu���Z�b�g����Ă���X�^�[�g�ʒu�ƈقȂ��Ă����
		if (Mouse::IsMouseButtonTrigger(MouseButton::LEFT) &&
			stage.startLaneZ != nowCursolPos.y) {
			//�R�}���h�ɒǉ�
			CommandDetail data;
			data.commandType = COMMAND_TYPE_SET_STARTLANE;
			data.args1 = stage.startLaneZ;
			data.args2 = nowCursolPos.y;
			Command::AddCommand(data);

			//�X�^�[�g�c�ʒu�ύX
			stage.startLaneZ = nowCursolPos.y;
		}
	}
	startLane[1].Update();
}

void Editor::DrawStartLane()
{
	//0...�ݒu���ꂽ�X�^�[�g���[��
	startLane[0].Draw();

	//�X�e�[�W�͈͊O�Ȃ�`�悵�Ȃ�
	if (IsInsideCursol() == false) {
		return;
	}

	//1...�J�[�\���ʒu�ɕ`�悳���X�^�[�g���[��
	if (mode == MODE_OPTION && optionMode == OPTION_SET_STARTLANE) {
		startLane[1].Draw();
	}
}

void Editor::DrawDispBlock()
{
	//�X�e�[�W�͈͊O�Ȃ�`�悵�Ȃ�
	if (IsInsideCursol() == false) {
		return;
	}

	objDispBlock.Draw();
}

void Editor::DrawDispFloor()
{
	//�X�e�[�W�͈͊O�Ȃ�`�悵�Ȃ�
	if (IsInsideCursol() == false) {
		return;
	}

	objDispFloor.Draw();
	//���f���Z�b�g����Ă���`��
	if (objDispFloorSub.GetObjModel() != nullptr) {
		objDispFloorSub.Draw();
	}
}

void Editor::Save()
{
	std::ofstream file;
	//�o�C�i�����[�h�ŊJ��
	file.open(STAGE_DIR + ioname + ".spb", std::ios_base::binary);
	//�t�@�C���I�[�v�����s�����o����
	assert(file.is_open());

	//�t�@�C���w�b�_�[
	StageHeader header = {};
	header.width = stage.stageSize.x;
	header.depth = stage.stageSize.y;
	header.startLineZ = stage.startLaneZ;
	header.blockCount = stage.blocks.size();
	header.floorCount = stage.floors.size();

	file.write((char*)&header, sizeof(header));

	//�f�[�^��
	//�u���b�N�ǉ�
	for (int i = 0; i < stage.blocks.size(); i++) {
		StageBlock object = {};

		object.type = stage.blocks[i]->GetBlockType();

		StageVec2 pos = GameUtility::CalcWorldPos2StagePos(
			stage.blocks[i]->GetPosition().x, stage.blocks[i]->GetPosition().z);
		object.stagePosX = pos.x;
		object.stagePosY = pos.y;
		object.breakupCount = stage.blocks[i]->GetBreakupCount();
		object.blockColor = stage.blocks[i]->GetBlockColor();

		file.write((char*)&object, sizeof(object));
	}

	//���ǉ�
	for (int i = 0; i < stage.floors.size(); i++) {
		StageFloor object = {};

		object.type = stage.floors[i]->GetFloorType();

		StageVec2 pos = GameUtility::CalcWorldPos2StagePos(
			stage.floors[i]->GetPosition().x, stage.floors[i]->GetPosition().z);

		object.stagePosX = pos.x;
		object.stagePosY = pos.y;

		file.write((char*)&object, sizeof(object));
	}

	//����
	file.close();
}

void Editor::Load()
{
	//�X�e�[�W���[�h
	stage.LoadStage(STAGE_DIR + ioname + ".spb");
	//ImGui�̃X���C�_�[�̒l�����������Ă���
	sliderWidth = stage.stageSize.x;
	sliderDepth = stage.stageSize.y;
	prevSize = stage.stageSize;
	//�X�^�[�g���[�����f���Đ���
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].CreateModel();
	}
	//�R�}���h������
	Command::Initialize();
}

void Editor::UpdateImgui()
{
	//ImGui
	ImguiHelper::BeginCommand("Settings");
	ImguiHelper::SetWindowSize(250, 720);
	ImguiHelper::SetWindowPos(1280 - 250, 0);
	//Mode
	ImGui::Text("Mode");
	ImGui::RadioButton("Add",		&mode, MODE_ADD);
	ImGui::SameLine();
	ImGui::RadioButton("Delete",	&mode, MODE_DELETE);
	ImGui::SameLine();
	ImGui::RadioButton("Option",	&mode, MODE_OPTION);

	if (mode == MODE_ADD) {
		//ObjectType
		ImGui::Text("ObjectType");
		ImGui::RadioButton("Block", &objectType, OBJECTTYPE_BLOCK);
		ImGui::SameLine();
		ImGui::RadioButton("Floor", &objectType, OBJECTTYPE_FLOOR);

		if (objectType == OBJECTTYPE_BLOCK) {
			ImGui::Text("BlockType");
			ImGui::RadioButton("Square",					&blockType, BLOCKTYPE_SQUARE);
			ImGui::RadioButton("Triangle_No_LeftTop",		&blockType, BLOCKTYPE_TRIANGLE_NO_LEFTTOP);
			ImGui::RadioButton("Triangle_No_RightTop",		&blockType, BLOCKTYPE_TRIANGLE_NO_RIGHTTOP);
			ImGui::RadioButton("Triangle_No_LeftBottom",	&blockType, BLOCKTYPE_TRIANGLE_NO_LEFTBOTTOM);
			ImGui::RadioButton("Triangle_No_RightBottom",	&blockType, BLOCKTYPE_TRIANGLE_NO_RIGHTBOTTOM);

			static int sliderBreakupCount = 0;
			ImGui::SliderInt("BreakupCount", &sliderBreakupCount, 0, 3);
			breakupCount = sliderBreakupCount;

			ImGui::Text("BlockColor");
			ImGui::RadioButton("None",		&blockColor, BLOCK_COLOR_NONE);
			ImGui::RadioButton("Red",		&blockColor, BLOCK_COLOR_RED);
			ImGui::RadioButton("Blue",		&blockColor, BLOCK_COLOR_BLUE);
			ImGui::RadioButton("Yellow",	&blockColor, BLOCK_COLOR_YELLOW);
			ImGui::RadioButton("Green",		&blockColor, BLOCK_COLOR_GREEN);
		}
		else if (objectType == OBJECTTYPE_FLOOR) {
			ImGui::Text("FloorType");
			ImGui::RadioButton("Normal",		&floorType, FLOORTYPE_NORMAL);
			ImGui::RadioButton("Turn_Left",		&floorType, FLOORTYPE_TURN_LEFT);
			ImGui::RadioButton("Turn_Right",	&floorType, FLOORTYPE_TURN_RIGHT);
			ImGui::RadioButton("Turn_Up",		&floorType, FLOORTYPE_TURN_UP);
			ImGui::RadioButton("Turn_Down",		&floorType, FLOORTYPE_TURN_DOWN);
			ImGui::RadioButton("Break",			&floorType, FLOORTYPE_BREAK);
			ImGui::RadioButton("Switch_None",	&floorType, FLOORTYPE_SWITCH_NONE);
			ImGui::RadioButton("Switch_Red",	&floorType, FLOORTYPE_SWITCH_RED);
			ImGui::RadioButton("Switch_Blue",	&floorType, FLOORTYPE_SWITCH_BLUE);
			ImGui::RadioButton("Switch_Yellow", &floorType, FLOORTYPE_SWITCH_YELLOW);
			ImGui::RadioButton("Switch_Green",	&floorType, FLOORTYPE_SWITCH_GREEN);
		}

	}
	else if (mode == MODE_DELETE) {
		//ObjectType
		ImGui::Text("ObjectType");
		ImGui::RadioButton("Block", &objectType, OBJECTTYPE_BLOCK);
		ImGui::SameLine();
		ImGui::RadioButton("Floor", &objectType, OBJECTTYPE_FLOOR);

		if (objectType == OBJECTTYPE_BLOCK) {
			ImGui::NewLine();
			if (ImGui::Button("All Delete")) {
				//�S�č폜
				for (int i = 0; i < stage.blocks.size(); i++) {
					if (stage.blocks[i]) {
						//�X�e�[�W��̍��W�擾
						StageVec2 pos = GameUtility::CalcWorldPos2StagePos(stage.blocks[i]->GetPosition().x, stage.blocks[i]->GetPosition().z);
						//�R�}���h�ɒǉ�
						CommandDetail data;
						data.commandType = COMMAND_TYPE_DELETE;
						data.args1 = pos.x;
						data.args2 = pos.y;
						data.objectType = OBJECTTYPE_BLOCK;
						data.blockFloorType = stage.blocks[i]->GetBlockType();
						data.breakupCount = stage.blocks[i]->GetBreakupCount();
						data.colorType = stage.blocks[i]->GetBlockColor();
						Command::AddCommand(data);

						delete stage.blocks[i];
					}
					stage.blocks.erase(stage.blocks.begin() + i);
					i--;
				}
			}
		}
		else if (objectType == OBJECTTYPE_FLOOR) {
			ImGui::NewLine();
			if (ImGui::Button("All Delete")) {
				//�S�č폜
				for (int i = 0; i < stage.floors.size(); i++) {
					if (stage.floors[i]) {
						//�X�e�[�W��̍��W�擾
						StageVec2 pos = GameUtility::CalcWorldPos2StagePos(stage.floors[i]->GetPosition().x, stage.floors[i]->GetPosition().z);
						//�R�}���h�ɒǉ�
						CommandDetail data;
						data.commandType = COMMAND_TYPE_DELETE;
						data.args1 = pos.x;
						data.args2 = pos.y;
						data.objectType = OBJECTTYPE_FLOOR;
						data.blockFloorType = stage.floors[i]->GetFloorType();
						Command::AddCommand(data);

						delete stage.floors[i]; 
					}
					stage.floors.erase(stage.floors.begin() + i);
					i--;
				}
			}
		}
	}
	else if (mode == MODE_OPTION) {
		//�X�e�[�W�T�C�Y�ύX
		ImGui::SliderInt("StageWidth", &sliderWidth, 5, 50);
		ImGui::SliderInt("StageDepth", &sliderDepth, 5, 50);

		//�I�v�V�������[�h�I��
		ImGui::Text("OptionMode");
		ImGui::RadioButton("SetStartLane", &optionMode, OPTION_SET_STARTLANE);
	}

	ImGui::NewLine();

	static char c[128] = "test";
	ImGui::InputText("IO_Name", c, sizeof(c));
	ioname = c;
	isEnteringIOName = ImGui::IsItemActive();

	if (ImGui::Button("Save")) {
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		Load();
	}

	ImguiHelper::EndCommand();
}

void Editor::CalcNowCursolPos()
{
	Vector3 mouse;
	//�}�E�X�ƒn�ʂƂ̓����蔻��
	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage.GetFloorCollision(), nullptr, &mouse);
	//DebugText::Print("WorldPos:(" + std::to_string(mouse.x) + ", " + std::to_string(mouse.z) + ")", 0, 0);
	StageVec2 newPos = GameUtility::CalcWorldPos2StagePos(mouse.x, mouse.z);
	nowCursolPos = newPos;

	//DebugText::Print("StagePos:(" + std::to_string(nowCursolPos.x) + ", " + std::to_string(nowCursolPos.y) + ")", 0, 20);
}

void Editor::ReCreateStage(const StageVec2& prevSize, const StageVec2& nowSize)
{
	//�X�e�[�W�ʒu�v�Z�Ɏg�p����X�e�[�W�T�C�Y���ꎞ�I�ɕύX
	StageVec2 calc = nowSize;
	GameUtility::SetPStageSize(&calc);

	//�܂��ϓ������X�e�[�W�T�C�Y�ɍ��킹�ăI�u�W�F�N�g�ړ�
	for (int i = 0; i < stage.blocks.size(); i++) {
		//CalcWorldPos2StagePos��stage.stageSize���Q�Ƃ����̂ŕύX�O�̒l����
		calc = prevSize;
		Vector3 wpos = stage.blocks[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		//SetStagePos�ŕύX��̍��W�ɂ��邽�߂ɐV�X�e�[�W�T�C�Y����
		calc = nowSize;
		stage.blocks[i]->SetStagePos(stagePos);
	}

	for (int i = 0; i < stage.floors.size(); i++) {
		//CalcWorldPos2StagePos��stage.stageSize���Q�Ƃ����̂ŕύX�O�̒l����
		calc = prevSize;
		Vector3 wpos = stage.floors[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		//SetStagePos�ŕύX��̍��W�ɂ��邽�߂ɐV�X�e�[�W�T�C�Y����
		calc = nowSize;
		stage.floors[i]->SetStagePos(stagePos);
	}

	//�u���b�N���X�e�[�W�O�ɔz�u����Ă�����폜
	for (int i = 0; i < stage.blocks.size(); i++) {

		Vector3 wpos = stage.blocks[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		if (stagePos.x < 0 || stagePos.x >= nowSize.x ||
			stagePos.y < 0 || stagePos.y >= nowSize.y) {
			if (stage.blocks[i]) delete stage.blocks[i];
			stage.blocks.erase(stage.blocks.begin() + i);
			i--;
		}
	}

	//���u���b�N���X�e�[�W�O�ɔz�u����Ă�����폜
	for (int i = 0; i < stage.floors.size(); i++) {

		Vector3 wpos = stage.floors[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		if (stagePos.x < 0 || stagePos.x >= nowSize.x ||
			stagePos.y < 0 || stagePos.y >= nowSize.y) {
			if (stage.floors[i]) delete stage.floors[i];
			stage.floors.erase(stage.floors.begin() + i);
			i--;
		}
	}

	//���������������u���b�N��ǉ�
	if (nowSize.x > prevSize.x){
		for (int i = 0; i < nowSize.y; i++) {
			for (int j = prevSize.x; j < nowSize.x; j++) {
				StageVec2 pos = { (unsigned short)j,(unsigned short)i };
				stage.AddFloor(pos, FLOORTYPE_NORMAL);
			}
		}
	}

	//�������c�������u���b�N��ǉ�
	if (nowSize.y > prevSize.y) {
		for (int i = prevSize.y; i < nowSize.y; i++) {
			for (int j = 0; j < nowSize.x; j++) {
				StageVec2 pos = { (unsigned short)j,(unsigned short)i };
				stage.AddFloor(pos, FLOORTYPE_NORMAL);
			}
		}
	}


	stage.startLaneZ = nowSize.y - 2;

	//�X�^�[�g���[�����f���Đ���
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].CreateModel();
	}

	//�X�e�[�W�ʒu�v�Z�Ɏg�p����X�e�[�W�T�C�Y��߂�
	GameUtility::SetPStageSize(&stage.stageSize);
}

bool Editor::IsInsideCursol()
{
	return nowCursolPos.x >= 0 && nowCursolPos.x < stage.stageSize.x &&
		nowCursolPos.y >= 0 && nowCursolPos.y < stage.stageSize.y;
}