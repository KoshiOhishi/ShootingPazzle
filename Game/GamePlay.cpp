#include "GamePlay.h"
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
	light.SetLightDir({ 1,1,1 });
	light.SetLightColor({ 1,1,1 });
	Object3D::SetLight(&light);

	//�J����������
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,50.0f,-70.0f }, 15.0f);
	camera.SetRotation(45, 0, 0);

	//�J�������Z�b�g
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//�^�C�}�[�Z�b�g
	timer.SetTimer(0, 1000000);
	timer.Start();

	//�X�e�[�W�擾
	stage.LoadStage("");

	//�e������
	myBullet.SetPBlocks(&stage);
	myBullet.Initialize();

	//�t�F�[�Y������
	GameUtility::SetNowPhase(PHASE_SETPOS);

}

void Editor::Update()
{
	// DirectX ���t���[������ ��������

	//���͏�����������

#pragma region �J����
#ifdef _DEBUG
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
#endif
#pragma endregion
#pragma region �T�E���h

#pragma endregion
#pragma region �I�u�W�F�N�g

#ifdef _DEBUG
	//���Z�b�g
	if (Keyboard::IsKeyTrigger(DIK_R)) {
		Initialize();
	}

#endif


#pragma endregion
#pragma region ImGui

	//ImGui
	ImguiHelper::BeginCommand("Settings");
	ImguiHelper::SetWindowSize(200, 100);
	ImguiHelper::SetWindowPos(1280 - 200, 0);

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
	
	ImguiHelper::EndCommand();

#pragma endregion
#pragma region �A�b�v�f�[�g
	light.Update();
	camera.Update();
	timer.Update();
	//3D�T�E���h�Ŏg�p���郊�X�i�[�̈ʒu�X�V
	Sound::Set3DListenerPosAndVec(camera);

	//�e�X�V
	myBullet.Update();
	//�X�e�[�W�X�V
	stage.Update();

#pragma endregion
}

void Editor::Draw()
{
	//�w�i�X�v���C�g�`�悱������
	Sprite::BeginDraw();
	//spr->Draw();

	//�w�i�`�悱���܂�
	DX12Util::ClearDepthBuffer();

	//�I�u�W�F�N�g�`�悱������

	//�e�`��
	myBullet.Draw();

	//�X�e�[�W�`��
	stage.Draw();
	

	//�I�u�W�F�N�g�`�悱���܂�

	//�O�i�X�v���C�g�`�悱������
	DebugText::DrawAll();
	ImguiHelper::Draw();

	//�O�i�X�v���C�g�`�悱���܂�

}