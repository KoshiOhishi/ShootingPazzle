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

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"

void GamePlay::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.15f, 720 * 0.15f, 0.1f, 150.0f);

	//�J����������
	camera.Initialize();
	//�J�������Z�b�g
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//���C�g������
	light.Initialize();
	light.SetLightDir({ 1,-1,1 });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(100.0f);
	//���C�g���Z�b�g
	Object3D::SetLight(&light);

	//�t�F�[�Y�Z�b�g
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//�X�e�[�W�擾
	stage.Initialize();
	stage.LoadStage(stagePass);

	//�X�e�[�W�T�C�Y����J�����ʒu�Z�b�g
	float bounceY = camera.SetPosFromStageSize(stage.GetStageSize());

	//�e������
	myBullet.SetPStage(&stage);
	myBullet.Initialize();
	myBullet.SetBounceInitPosY(bounceY);

	//�w�i������
	modelBG.CreateFromOBJ("sky");
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	//�X�e�[�W�J���[������
	GameUtility::SetStageColor(STAGE_COLOR_NONE);
}

void GamePlay::Update()
{
	// DirectX ���t���[������ ��������

	//���͏�����������

#pragma region �J����
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

#pragma endregion
#pragma region �T�E���h

#pragma endregion
#pragma region �I�u�W�F�N�g

	objBG.AddRotation(0, 0.1f, 0);

	//���Z�b�g
	Reset();

	//�N���A���Ă��邩�H
	CheckIsClear();

#pragma endregion
#pragma region ImGui

	//ImGui
	ImguiHelper::BeginCommand("Settings");
	ImguiHelper::SetWindowSize(300, 300);
	ImguiHelper::SetWindowPos(1280 - 300, 0);

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
	camera.Update();
	light.Update();
	objBG.Update();
	//3D�T�E���h�Ŏg�p���郊�X�i�[�̈ʒu�X�V
	Sound::Set3DListenerPosAndVec(camera);

	//�e�X�V
	myBullet.Update();
	//�X�e�[�W�X�V
	stage.Update();

#pragma endregion
}

void GamePlay::Draw()
{
	//�w�i�`��
	objBG.Draw();

	//�X�e�[�W�`��
	stage.Draw();

	//�e�`��
	myBullet.Draw();
}

void GamePlay::Reset()
{
	if (Keyboard::IsKeyTrigger(DIK_R)) {
		//�X�e�[�W�擾
		stage.LoadStage(stagePass);

		//�e������
		myBullet.Initialize();

		//�O�̂��߃J�������ʒu��
		camera.SetCameraParamAfterShoot();

		//�t�F�[�Y������
		GameUtility::SetNowPhase(PHASE_GAME_SETPOS);

		//�X�e�[�W�J���[������
		GameUtility::SetStageColor(STAGE_COLOR_NONE);
	}

	//�f�o�b�O�p�@�����ł͏���
	if (Keyboard::IsKeyTrigger(DIK_Q)) {

		GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

		//�X�e�[�W�擾
		stage.LoadStage(stagePass);

		//�e������
		myBullet.Initialize();

		//�J�����������@�������͂���
		camera.Initialize();

		//�X�e�[�W�T�C�Y����J�����ʒu�Z�b�g
		camera.SetPosFromStageSize(stage.GetStageSize());

		//�X�e�[�W�J���[������
		GameUtility::SetStageColor(STAGE_COLOR_NONE);
	}
}

void GamePlay::CheckIsClear()
{
	//�c��ڕW�j��u���b�N��0��������N���A
	if (GameUtility::GetNowPhase() == PHASE_GAME_AFTERSHOOT && 
		stage.GetTargetBlockCount() <= 0) {
		GameUtility::SetNowPhase(PHASE_GAME_CLEAR);
		//�e��N���A�G�t�F�N�g�^�C�}�[�X�^�[�g
		stage.StartEffectTimer(0, 10000);
		camera.StartEffectTimer(0, 5000);
	}
}
