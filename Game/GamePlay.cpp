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
#include "SceneManager.h"
#include "Easing.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"

GamePlay::GamePlay()
{
	modelBG.CreateFromOBJ(modelDir + "Sky/Sky.obj");
	buttonReset.LoadTexture(L"Resources/UI/UI_Arrow_Reset.png");
	buttonBack.LoadTexture(L"Resources/UI/UI_Arrow_Back.png");
	buttonYes.LoadTexture(L"Resources/UI/UI_Yes.png");
	buttonNo.LoadTexture(L"Resources/UI/UI_No.png");
	sprWhite.Initialize();
	sprWhite.SetTexture(L"Resources/Write1280x720.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(L"Resources/Black1280x720.png");
	sprPopUp.Initialize();
	sprPopUp.SetTexture(L"Resources/Game_PopUp.png");
	for (int i = 0; i < _countof(sprClearText); i++) {
		sprClearText[i].Initialize();
		sprClearText[i].SetTexture(L"Resources/ClearText.png");
	}
}

GamePlay::~GamePlay()
{
}

void GamePlay::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//�t�F�[�Y�Z�b�g
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//�|�[�Y����
	GameUtility::SetIsPause(false);

	//�J����������
	camera.Initialize();
	camera.SetPFirstEffectTimer(&firstEffectTimer);
	camera.SetPClearEffectTimer(&clearEffectTimer);

	//�J�������Z�b�g
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//���C�g������
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(80.0f);
	//���C�g���Z�b�g
	Object3D::SetLight(&light);

	//�X�e�[�W�擾
	stage.Initialize();
	stage.SetPFirstEffectTimer(&firstEffectTimer);
	stage.SetPClearEffectTimer(&clearEffectTimer);
	stage.LoadStage(GameUtility::GetNowStagePath());

	//�X�e�[�W�T�C�Y����J�����ʒu�Z�b�g
	float bounceY = camera.SetPosFromStageSize(stage.GetStageSize());

	//�e������
	myBullet.SetPStage(&stage);
	myBullet.Initialize();
	myBullet.SetBounceInitPosY(bounceY);
	myBullet.SetPFirstEffectTimer(&firstEffectTimer);

	//�w�i�I�u�W�F�N�g������
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	//�|�b�v�A�b�v������
	sprPopUp.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprPopUp.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.5f - sprPopUp.GetTexSize().y * 0.5f });
	
	//������Ԃ͓���
	sprPopUp.SetColor({ 1,1,1,0 });
	isDispPopup = false;

	sprBlack.SetColor({ 1,1,1,0});

	//�uClear�v�����̏����ʒu(��ʗ��[)
	float clearTextPosY = 150;
	sprClearText[0].SetPosition({ -sprClearText[0].GetTexSize().x, clearTextPosY });
	sprClearText[1].SetPosition({ (float)DX12Util::GetWindowWidth(), clearTextPosY });

	//�^�C�}�[������
	firstEffectTimer.SetTimer(0, 4250);
	firstEffectTimer.Start();
	dispPopUpTimer.SetTimer(0, 500);
	sceneChangeTimer.SetTimer(0, 1000);
	clearEffectTimer.SetTimer(0, 10000);

	//UI�{�^��������
	float adjust = 10;
	buttonReset.Initialize({ -buttonReset.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - buttonReset.GetTexSize().y - adjust * 2 });
	buttonBack.Initialize({ -buttonBack.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - adjust });
	buttonYes.Initialize({ DX12Util::GetWindowWidth() * 0.375f - buttonYes.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.75f - buttonYes.GetTexSize().y * 0.5f});
	//������Ԃ͓���
	buttonYes.SetColor({ 1,1,1,0 });
	buttonNo.Initialize({ DX12Util::GetWindowWidth() * 0.625f - buttonNo.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.75f - buttonNo.GetTexSize().y * 0.5f });
	//������Ԃ͓���
	buttonNo.SetColor({ 1,1,1,0 });


	//�X�e�[�W�J���[������
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//�J���G�t�F�N�g�I���t���O
	isEndFirstEffectOnce = false;
}

void GamePlay::Update()
{
	UpdateTimer();
	UpdateDebugCamera();
	camera.Update();
	light.Update();
	//3D�T�E���h�Ŏg�p���郊�X�i�[�̈ʒu�X�V
	Sound::Set3DListenerPosAndVec(camera);
	//UpdateImgui();
	//UI�X�V
	UpdateUI();
	//�|�b�v�A�b�v�X�V
	UpdateStageBackPopUp();

	//�N���A���Ă��邩�H
	CheckIsClear();
	//�w�i�I�u�W�F�N�g�X�V
	UpdateBG();
	//�e�X�V
	myBullet.Update();
	//�X�e�[�W�X�V
	stage.Update();

	//���G�t�F�N�g�X�V
	UpdateFirstEffect();

	//�N���A�G�t�F�N�g�X�V
	UpdateClearEffect();
	
}

void GamePlay::Draw()
{
	//�w�i�`��
	objBG.Draw();

	//�e�`��
	myBullet.Draw();

	//�X�e�[�W�`��
	stage.Draw();

	//UI�`��
	DrawUI();

	//���e�N�X�`���`��
	DrawBlackEffect();

	//�N���A�G�t�F�N�g�`��
	DrawClearEffect();

	//�|�b�v�A�b�v�`��
	DrawStageBackPopUp();

	//���e�N�X�`���`��
	DrawWhiteEffect();

}

void GamePlay::UpdateDebugCamera()
{
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
#endif // _DEBUG
}

void GamePlay::UpdateTimer()
{
	//�J���G�t�F�N�g���I�������狅�̈ʒu���߃t�F�[�Y��
	if (firstEffectTimer.GetIsEnd() == true &&
		GameUtility::GetNowPhase() == PHASE_GAME_FIRSTEFFECT) {
		GameUtility::SetNowPhase(PHASE_GAME_SETPOS);
		isEndFirstEffectOnce = true;
	}

	firstEffectTimer.Update();
	dispPopUpTimer.Update();
	sceneChangeTimer.Update();
	clearEffectTimer.Update();
}

void GamePlay::UpdateFirstEffect()
{
	//���S�ɓ����łȂ���΍X�V
	if (firstEffectTimer.GetIsEnd() == true) {
		return;
	}

	//���e�N�X�`�������x�X�V
	float alpha = Easing::GetEaseValue(EASE_INOUTEXPO, 1, 0, firstEffectTimer, 0, 500);
	Vector4 color = sprWhite.GetColor();
	color.w = alpha;
	sprWhite.SetColor(color);
}

void GamePlay::UpdateBG()
{
	objBG.AddRotation(0, 0.1f, 0);
	objBG.Update();
}

void GamePlay::UpdateImgui()
{
#ifdef _DEBUG
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
#endif
}

void GamePlay::UpdateUI()
{
	//�J���G�t�F�N�g�̈ʒu�ړ�(1�񂾂�)
	if (isEndFirstEffectOnce == false) {
		float x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonReset.GetTexSize().x, 10, firstEffectTimer, 3500, 4000);
		buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
		x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonBack.GetTexSize().x, 10, firstEffectTimer, 3750, 4250);
		buttonBack.SetPosition({ x, buttonBack.GetPosition().y });
	}
	//�{�^������
	if (GameUtility::GetIsPause() == false) {
		buttonReset.SetIsDispOverlapMouseTex(true);
		buttonBack.SetIsDispOverlapMouseTex(true);

		if (buttonReset.IsReleaseButton()) {
			buttonReset.StartPushedEffect();
			Reset();
		}
		if (buttonBack.IsReleaseButton()) {
			buttonBack.StartPushedEffect();
			//�|�b�v�A�b�v�^�C�}�[�X�^�[�g
			dispPopUpTimer.Reset();
			dispPopUpTimer.Start();
			isDispPopup = true;
			//�|�[�Y��Ԃɂ���
			GameUtility::SetIsPause(true);
		}
	}
	else {
		//�|�[�Y���̓J�[�\���ƃ{�^�����d�Ȃ��Ă��e�N�X�`����ω������Ȃ�
		if (dispPopUpTimer.GetIsEnd()) {
			buttonReset.SetIsDispOverlapMouseTex(false);
			buttonBack.SetIsDispOverlapMouseTex(false);
		}

	}
}

void GamePlay::UpdateStageBackPopUp()
{

	if (GameUtility::GetIsPause()) {
		//�|�b�v�A�b�v�̓����x�ω�
		if (dispPopUpTimer.GetIsStart()) {
			float alpha = 0;
			//�|�b�v�A�b�v�\���I���Ȃ�0����1��
			if (isDispPopup) {
				alpha = Easing::GetEaseValue(EASE_OUTQUINT, 0, 1, dispPopUpTimer);
			}
			//�����łȂ��Ȃ�1��0��
			else {
				alpha = Easing::GetEaseValue(EASE_OUTQUINT, 1, 0, dispPopUpTimer);
			}
			sprPopUp.SetColor({ 1,1,1, alpha });
			sprBlack.SetColor({ 1,1,1, alpha * 0.5f });
			buttonYes.SetColor({ 1,1,1, alpha });
			buttonNo.SetColor({ 1,1,1, alpha });
		}

		//�V�[���`�F���W���Ă��Ȃ���΃{�^����t
		if (sceneChangeTimer.GetIsStart() == false) {
			if (buttonYes.IsReleaseButton()) {
				buttonYes.StartPushedEffect();
				sceneChangeTimer.Start();
			}
			if (buttonNo.IsReleaseButton()) {
				buttonNo.StartPushedEffect();
				dispPopUpTimer.Reset();
				dispPopUpTimer.Start();
				isDispPopup = false;
			}
		}
		
		//�|�b�v�A�b�v�^�C�}�[�I���Ń|�[�Y����
		if (isDispPopup == false && dispPopUpTimer.GetIsEnd()) {
			GameUtility::SetIsPause(false);
		}
	}

	//�V�[���`�F���W�X�V
	if (sceneChangeTimer.GetIsStart()) {
		//���e�N�X�`�������x�X�V
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 1, sceneChangeTimer, 0, 1000);
		Vector4 color = sprWhite.GetColor();
		color.w = alpha;
		sprWhite.SetColor(color);
	}

	//�^�C�}�[�I���ŃV�[���`�F���W���s
	if (sceneChangeTimer.GetIsEnd()) {
		SceneManager::ChangeScene("StageSelect");
	}
}

void GamePlay::UpdateClearEffect()
{
	//�N���A�G�t�F�N�g���̂ݍX�V
	if (clearEffectTimer.GetNowTime() <= clearEffectTimer.GetStartTime()) {
		return;
	}

	const int END_MOVE_TEXT_TIME = 5250;

	//�N���A��������ʂ̗��[���璆�S��
	for (int i = 0; i < _countof(sprClearText); i++) {
		//�e�N�X�`���ړ��J�n�I���ʒu
		float startPosX = 0, endPosX = DX12Util::GetWindowWidth() * 0.5f - sprClearText[i].GetTexSize().x * 0.5f;
		if (i == 0) { startPosX = -sprClearText[i].GetTexSize().x; }
		else { startPosX = DX12Util::GetWindowWidth(); }

		Vector2 texPos = { startPosX, 150 };
		texPos.x = Easing::GetEaseValue(EASE_LINE, startPosX, endPosX, clearEffectTimer, 5000, END_MOVE_TEXT_TIME);

		sprClearText[i].SetPosition(texPos);
	}

	//���e�N�X�`���ƍ��e�N�X�`��
	if (clearEffectTimer.GetNowTime() >= END_MOVE_TEXT_TIME + 500) {
		//���e�N�X�`�������x�X�V
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 0.5f, clearEffectTimer, END_MOVE_TEXT_TIME + 500, END_MOVE_TEXT_TIME + 1000);
		Vector4 color = sprBlack.GetColor();
		color.w = alpha;
		sprBlack.SetColor(color);
	}
	else{
		sprBlack.SetColor({ 1,1,1,0 });
	}
}

void GamePlay::DrawWhiteEffect()
{
	sprWhite.DrawFG();
}

void GamePlay::DrawBlackEffect()
{
	sprBlack.DrawFG();
}

void GamePlay::DrawUI()
{
	buttonReset.Draw();
	buttonBack.Draw();
}

void GamePlay::DrawStageBackPopUp()
{
	//�G�t�F�N�g���������̓|�[�Y���ɕ`��
	if (GameUtility::GetIsPause() || dispPopUpTimer.GetIsEnd() == false) {
		sprPopUp.DrawFG();
		buttonYes.Draw();
		buttonNo.Draw();
	}
}

void GamePlay::DrawClearEffect()
{
	//�N���A�G�t�F�N�g���̂ݕ`��
	if (clearEffectTimer.GetNowTime() <= clearEffectTimer.GetStartTime()) {
		return;
	}

	//���������e�N�X�`��
	sprBlack.DrawFG();

	//�uClear�v����
	for (int i = 0; i < _countof(sprClearText); i++) {
		sprClearText[i].DrawFG();
	}
}

void GamePlay::Reset()
{
	//�J���G�t�F�N�g���̓��Z�b�g�����Ȃ�
	if (GameUtility::GetNowPhase() == PHASE_GAME_FIRSTEFFECT) {
		return;
	}

	//if (Keyboard::IsKeyTrigger(DIK_R)) {
	//�t�F�[�Y������
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//�X�e�[�W�擾
	stage.LoadStage(GameUtility::GetNowStagePath());

	//���̃G�t�F�N�g�̂ݔ��f����悤�Ȕ͈�
	firstEffectTimer.SetTimer(MYBULLET_START_FIRST_EFFECT_TIME, 4250);
	firstEffectTimer.Start();

	//�N���A�G�t�F�N�g�^�C�}�[���Z�b�g
	clearEffectTimer.Reset();

	//�e������
	myBullet.Initialize();

	//�O�̂��߃J�������ʒu��
	camera.SetCameraParamAfterShoot();

	//�O�̂��ߔ��ƍ��e�N�X�`�������x0��
	sprWhite.SetColor({ 1,1,1,0 });
	sprBlack.SetColor({ 1,1,1,0 });

	//�X�e�[�W�J���[������
	GameUtility::SetStageColor(STAGE_COLOR_NONE);
	//}

	//�f�o�b�O�p�@�����ł͏���
	//if (Keyboard::IsKeyTrigger(DIK_Q)) {

	//	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//	//�X�e�[�W�擾
	//	stage.Initialize();
	//	stage.LoadStage(GameUtility::GetNowStagePath());

	//	//�e������
	//	myBullet.Initialize();

	//	//�J�����������@�������͂���
	//	camera.Initialize();

	//	//�X�e�[�W�T�C�Y����J�����ʒu�Z�b�g
	//	camera.SetPosFromStageSize(stage.GetStageSize());

	//	//�X�e�[�W�J���[������
	//	GameUtility::SetStageColor(STAGE_COLOR_NONE);
	//}
}

void GamePlay::CheckIsClear()
{
	//�c��ڕW�j��u���b�N��0��������N���A
	if (GameUtility::GetNowPhase() == PHASE_GAME_AFTERSHOOT && 
		stage.GetTargetBlockCount() <= 0) {
		GameUtility::SetNowPhase(PHASE_GAME_CLEAR);
		//�e��N���A�G�t�F�N�g�^�C�}�[�X�^�[�g
		if (clearEffectTimer.GetIsStart() == false) {
			clearEffectTimer.Start();
		}
	}
}
