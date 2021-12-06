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

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

void Editor::Initialize()
{
	//���C�g������
	light.Initialize();
	light.SetLightDir({ 1,-1,0 });
	light.SetLightColor({ 0.8f, 0.8f, 0.8f });
	Object3D::SetLight(&light);

	//�J����������
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,100.0f,0.0f }, 30.0f);
	camera.SetRotation(90, 0, 0);

	//�J�������Z�b�g
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//�^�C�}�[�Z�b�g
	timer.SetTimer(0, 1000000);
	timer.Start();

	//�X�e�[�W�擾
	stage.LoadStage("");

	//�t�F�[�Y������
	GameUtility::SetNowPhase(PHASE_SETPOS);

	//�I�u�W�F�N�g������
	squareBlock.Initialize(0, 0, ONE_CELL_LENGTH / 2);
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].Initialize(0, 0, ONE_CELL_LENGTH / 2);
		triangleBlock[i].SetShapeType(i);
	}
}

void Editor::Update()
{
	// DirectX ���t���[������ ��������

	//���͏�����������

#pragma region �J����
#ifdef _DEBUG
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

		//�J������]
		if (Keyboard::IsKeyPush(DIK_UP)) {
			Vector3 f = { camera.GetRotation().x - 1.5f, camera.GetRotation().y, camera.GetRotation().z };
			camera.SetRotation(f);
		}
		if (Keyboard::IsKeyPush(DIK_DOWN)) {
			Vector3 f = { camera.GetRotation().x + 1.5f, camera.GetRotation().y, camera.GetRotation().z };
			camera.SetRotation(f);
		}
		if (Keyboard::IsKeyPush(DIK_LEFT)) {
			//Shift��������Ă�����Z����]
			if (Keyboard::IsKeyPush(DIK_LSHIFT) || Keyboard::IsKeyPush(DIK_RSHIFT)) {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z + 1.5f };
				camera.SetRotation(f);
			}
			else {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y - 1.5f, camera.GetRotation().z };
				camera.SetRotation(f);
			}
		}
		if (Keyboard::IsKeyPush(DIK_RIGHT)) {
			if (Keyboard::IsKeyPush(DIK_LSHIFT) || Keyboard::IsKeyPush(DIK_RSHIFT)) {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z - 1.5f };
				camera.SetRotation(f);
			}
			else {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y + 1.5f, camera.GetRotation().z };
				camera.SetRotation(f);
			}
		}
	}
#endif
#pragma endregion
#pragma region �T�E���h

#pragma endregion
#pragma region �I�u�W�F�N�g
	UpdateEdit();
#pragma endregion
#pragma region �A�b�v�f�[�g
	light.Update();
	camera.Update();
	timer.Update();
	//3D�T�E���h�Ŏg�p���郊�X�i�[�̈ʒu�X�V
	Sound::Set3DListenerPosAndVec(camera);
	//�X�e�[�W�X�V
	stage.Update();

#pragma endregion

#ifdef _DEBUG
	//���Z�b�g
	if (Keyboard::IsKeyTrigger(DIK_R) && isEnteringIOName == false) {
		Initialize();
	}

#endif


}

void Editor::Draw()
{
	//�w�i�X�v���C�g�`�悱������
	Sprite::BeginDraw();
	//spr->Draw();

	//�w�i�`�悱���܂�
	DX12Util::ClearDepthBuffer();

	//�I�u�W�F�N�g�`�悱������

	DrawEdit();

	//�I�u�W�F�N�g�`�悱���܂�

	//�O�i�X�v���C�g�`�悱������
	DebugText::DrawAll();
	ImguiHelper::Draw();

	//�O�i�X�v���C�g�`�悱���܂�

}

void Editor::UpdateEdit()
{
	UpdateImgui();
	CalcNowCursolPos();
	UpdateObject();

	if (mode == MODE_ADD) {
		UpdateAdd();
	}
	else if (mode == MODE_DELETE) {
		UpdateDelete();
	}

}

void Editor::UpdateAdd()
{
	//�X�e�[�W�͈͊O�Ȃ瑦���^�[��
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.width ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.depth) {
		return;
	}

	if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
		stage.AddBlock(nowCursolPos, blockType, shapeType);
	}
}

void Editor::UpdateDelete()
{
	//�X�e�[�W�͈͊O�Ȃ瑦���^�[��
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.width ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.depth) {
		return;
	}

	if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
		stage.DeleteBlock(nowCursolPos);
	}
}

void Editor::UpdateObject()
{
	//�u���b�N�ʒu���J�[�\���ʒu��
	float x, z;
	GameUtility::CalcStagePos2WorldPos(nowCursolPos, stage.width, stage.depth, &x, &z);
	squareBlock.SetBlockPos(x, z);
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].SetBlockPos(x, z);
	}

	//�X�V
	squareBlock.Update();
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].Update();
	}
}

void Editor::DrawEdit()
{
	//�X�e�[�W�`��
	stage.Draw();

	if (mode == MODE_ADD) {
		//�J�[�\���ʒu�Ƀu���b�N�`��
		DrawBlock();
	}
	else if (mode == MODE_DELETE) {

	}
}

void Editor::DrawBlock()
{
	//�X�e�[�W�͈͊O�Ȃ瑦���^�[��
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.width ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.depth) {
		return;
	}

	if (blockType == BLOCKTYPE_SQUARE) {
		squareBlock.Draw();
	}
	else if (blockType == BLOCKTYPE_TRIANGLE) {
		triangleBlock[shapeType].Draw();
	}
}

void Editor::Save()
{
	std::ofstream file;
	//�o�C�i�����[�h�ŊJ��
	file.open("./StageData/" + ioname + ".spb", std::ios_base::binary);
	//�t�@�C���I�[�v�����s�����o����
	assert(file.is_open());

	//�t�@�C���w�b�_�[
	StageHeader header = {};
	header.width = stage.width;
	header.depth = stage.depth;
	header.objectCount = stage.blocks.size();

	file.write((char*)&header, sizeof(header));

	//�f�[�^��
	for (int i = 0; i < stage.blocks.size(); i++) {
		StageObject object = {};
		std::string blockType = stage.blocks[i]->GetObjectType();

		if (blockType == "SquareBlock") {
			object.type = 0;
		}
		else if (blockType == "TriangleBlock_0") {
			object.type = 1;
		}
		else if (blockType == "TriangleBlock_1") {
			object.type = 2;
		}
		else if (blockType == "TriangleBlock_2") {
			object.type = 3;
		}
		else if (blockType == "TriangleBlock_3") {
			object.type = 4;
		}

		StagePos pos = GameUtility::CalcWorldPos2StagePos(
			stage.blocks[i]->GetPosition().x, stage.blocks[i]->GetPosition().z, stage.width, stage.depth);
		object.stagePosX = pos.x;
		object.stagePosY = pos.y;

		file.write((char*)&object, sizeof(object));
	}

	file.close();
}

void Editor::Load()
{
	stage.LoadStage("./StageData/" + ioname + ".spb");
}

void Editor::UpdateImgui()
{
	//ImGui
	ImguiHelper::BeginCommand("Settings");
	ImguiHelper::SetWindowSize(200, 720);
	ImguiHelper::SetWindowPos(1280 - 200, 0);
	ImGui::Text("Mode");
	ImGui::RadioButton("Add", &mode, MODE_ADD);
	ImGui::SameLine();
	ImGui::RadioButton("Delete", &mode, MODE_DELETE);
	if (mode == MODE_ADD) {
		ImGui::Text("BlockType");
		ImGui::RadioButton("Square", &blockType, BLOCKTYPE_SQUARE);
		ImGui::SameLine();
		ImGui::RadioButton("Triangle", &blockType, BLOCKTYPE_TRIANGLE);

		if (blockType == BLOCKTYPE_TRIANGLE) {
			ImGui::Text("TriangleType");
			ImGui::RadioButton("No_LeftTop", &shapeType, SHAPETYPE_NO_LEFTTOP);
			ImGui::RadioButton("No_RightTop", &shapeType, SHAPETYPE_NO_RIGHTTOP);
			ImGui::RadioButton("No_LeftBottom", &shapeType, SHAPETYPE_NO_LEFTBOTTOM);
			ImGui::RadioButton("No_RightBottom", &shapeType, SHAPETYPE_NO_RIGHTBOTTOM);
		}
	}
	else if (mode == MODE_DELETE) {
		ImGui::NewLine();
		if (ImGui::Button("All Delete")) {
			//�S�ď�����
			stage.LoadStage("");
			//��قǂ����Ƀ}�b�v�̑傫����ς��鏈��
		}
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
	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage.floor.GetPlane(), nullptr, &mouse);
	//DebugText::Print("WorldPos:(" + std::to_string(mouse.x) + ", " + std::to_string(mouse.z) + ")", 0, 0);
	StagePos newPos = GameUtility::CalcWorldPos2StagePos(mouse.x, mouse.z, stage.width, stage.depth);
	nowCursolPos = newPos;

	//DebugText::Print("StagePos:(" + std::to_string(nowCursolPos.x) + ", " + std::to_string(nowCursolPos.y) + ")", 0, 20);
}

//bool blnChk = false;
//ImGui::Checkbox("CheckboxTest", &blnChk);
//int radio = 0;
//ImGui::RadioButton("Radio 1", &radio, 0);
//ImGui::SameLine();
//ImGui::RadioButton("Radio 2", &radio, 1);
//ImGui::SameLine();
//ImGui::RadioButton("Radio 3", &radio, 2);
//int nSlider = 0;
//ImGui::SliderInt("Int Slider", &nSlider, 0, 100);
//float fSlider = 0.0f;
//ImGui::SliderFloat("Float Slider", &fSlider, 0.0f, 100.0f);
//float col3[3] = {};
//ImGui::ColorPicker3("ColorPicker3", col3, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
//float col4[4] = {};
//ImGui::ColorPicker4(" ColorPicker4", col4, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | 
//ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
