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
#include "GameSound.h"
#include "Encorder.h"

using namespace DX12Library;

GamePlay::GamePlay()
{
	modelBG.CreateFromOBJ(MODEL_DIR + "Sky/Sky.obj");
	buttonReset.LoadTexture(TEX_DIR_GAMEPLAY + "UI_Arrow_Reset.png");
	buttonBack.LoadTexture(TEX_DIR_GAMEPLAY + "UI_Arrow_Back.png");
	buttonYes.LoadTexture(TEX_DIR_GAMEPLAY + "UI_Yes.png");
	buttonNo.LoadTexture(TEX_DIR_GAMEPLAY + "UI_No.png");
	buttonOK.LoadTexture(TEX_DIR_GAMEPLAY + "UI_OK.png");
	sprWhite.Initialize();
	sprWhite.SetTexture(TEX_DIR_UTIL + "White1280x720.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(TEX_DIR_UTIL + "Black1280x720.png");
	sprPopUp.Initialize();
	sprPopUp.SetTexture(TEX_DIR_GAMEPLAY + "Game_PopUp.png");
	sprUIRemainingBlock.Initialize();
	sprUIRemainingBlock.SetTexture(TEX_DIR_GAMEPLAY + "UI_RemainingBlock.png");
	for (int i = 0; i < _countof(sprRemainingBlockCount); i++) {
		sprRemainingBlockCount[i].Initialize();
		sprRemainingBlockCount[i].SetTexture(TEX_DIR_GAMEPLAY + "UI_Number.png");
	}
	for (int i = 0; i < _countof(sprTextClear); i++) {
		sprTextClear[i].Initialize();
		sprTextClear[i].SetTexture(TEX_DIR_GAMEPLAY + "Text_Clear.png");
	}
	sprTextClearTime.Initialize();
	sprTextClearTime.SetTexture(TEX_DIR_GAMEPLAY + "Text_ClearTime.png");
	for (int i = 0; i < _countof(sprTextTimeNumber); i++) {
		sprTextTimeNumber[i].Initialize();
		sprTextTimeNumber[i].SetTexture(TEX_DIR_GAMEPLAY + "Text_TimeNumber.png");
	}
	particle[0].LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Shine.png");
	particle[1].LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Shine.png");
}

GamePlay::~GamePlay()
{
}

void GamePlay::Initialize()
{
	//�t�F�[�Y�Z�b�g
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//�|�[�Y����
	GameUtility::SetIsPause(false);

	//�J����������
	camera.Initialize();
	camera.SetPFirstEffectTimer(&firstEffectTimer);
	camera.SetPClearEffectTimer(&clearEffectTimer);

	//�J�������Z�b�g
	DX12Util::SetCameraAll(&camera);

	//���C�g������
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	//���C�g���Z�b�g
	Object3D::SetLight(&light);

	//�X�e�[�W�擾
	stage.Initialize();
	stage.SetPFirstEffectTimer(&firstEffectTimer);
	stage.SetPClearEffectTimer(&clearEffectTimer);
	stage.LoadStage(GameUtility::GetNowStagePath());

	//�X�e�[�W�T�C�Y����J�����ʒu�Z�b�g
	float bounceY = camera.SetPosFromStageSize(stage.GetStageSize());

	//�X�e�[�W�T�C�Y���烉�C�g�ʒu�ƍs��Z�b�g
	//��������������ɂ���
	float length = stage.GetStageSize().x < stage.GetStageSize().y ?
		(float)stage.GetStageSize().y : (float)stage.GetStageSize().x;

	//�s����Z�b�g
	light.CalcLightPos(length * 2.5);
	float mul = length * 0.012f;
	float farZ = length * 4.5 + 5;
	Object3D::SetMatrixOrthographicLH(1280 * mul, 720 * mul, 1.0f, farZ);

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

	//UI�ɂ���u���b�N
	objUISquareBlock.Initialize();
	//��ԃ{���{���Ȍ����ڂ̃��f�����擾
	objUISquareBlock.SetObjModel(SquareBlock::GetModel(1));
	objUISquareBlock.SetScale(0.075, 0.075, 0.075);
	//�e�`�斳��
	objUISquareBlock.SetDrawShadowToOther(false);
	objUISquareBlock.SetDrawShadowToMyself(false);

	//�|�b�v�A�b�v�\���t���O
	isDispPopup = false;

	//�|�b�v�A�b�v������
	sprPopUp.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprPopUp.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.5f - sprPopUp.GetTexSize().y * 0.5f });
	
	//������Ԃ͓���
	sprPopUp.SetColor({ 1,1,1,0 });
	sprWhite.SetColor({ 1,1,1,1 });
	sprBlack.SetColor({ 1,1,1,0});
	sprTextClearTime.SetPosition({ 175,450 });
	sprUIRemainingBlock.SetPosition({ -sprUIRemainingBlock.GetTexSize().x, 95.0f });

	//�uClear�v�����̏����ʒu(��ʗ��[)
	float clearTextPosY = 150;
	sprTextClear[0].SetPosition({ -sprTextClear[0].GetTexSize().x, clearTextPosY });
	sprTextClear[1].SetPosition({ (float)DX12Util::GetWindowWidth(), clearTextPosY });

	//�^�C�}�[������
	firstEffectTimer.SetTimer(0, 4500);
	firstEffectTimer.Start();
	dispPopUpTimer.SetTimer(0, 500);
	sceneChangeTimer.SetTimer(0, 1000);
	clearEffectTimer.SetTimer(0, 10000);
	scoreTimer.SetTimer(0, INT_MAX);
	addParticleTimer.SetTimer(0, 100);

	//UI�{�^��������
	float adjust = 10;
	buttonReset.Initialize({ -buttonReset.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - buttonReset.GetTexSize().y - adjust * 2 });
	buttonBack.Initialize({ -buttonBack.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - adjust });
	buttonYes.Initialize({ DX12Util::GetWindowWidth() * 0.375f - buttonYes.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.75f - buttonYes.GetTexSize().y * 0.5f});
	buttonOK.Initialize({ (float)DX12Util::GetWindowWidth(), (float)DX12Util::GetWindowHeight() - buttonOK.GetTexSize().y - adjust });
	//������Ԃ͓���
	buttonYes.SetColor({ 1,1,1,0 });
	buttonNo.Initialize({ DX12Util::GetWindowWidth() * 0.625f - buttonNo.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.75f - buttonNo.GetTexSize().y * 0.5f });
	//������Ԃ͓���
	buttonNo.SetColor({ 1,1,1,0 });

	//�p�[�e�B�N��������
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Initialize();
		particle[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}

	addedParticleClearEffect = false;

	//�X�e�[�W�J���[������
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//�J���G�t�F�N�g�I���t���O
	isEndFirstEffectOnce = false;

	//�`���[�g���A��
	tutorial.Initialize(true);

	//BGM���ĂȂ�������Đ�
	if (GameSound::IsPlaying(L"GamePlay") == false) {
		GameSound::Play(L"GamePlay");
	}
}

void GamePlay::Update()
{
	UpdateTimer();
	UpdateDebugCamera();
	camera.Update();
	light.Update();
	//3D�T�E���h�Ŏg�p���郊�X�i�[�̈ʒu�X�V
	Sound::Set3DListenerPosAndVec(camera);

	//�T�E���h�X�V
	GameSound::Update();

	//UpdateImgui();
	//�`���[�g���A��
	tutorial.Update();
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

	//�V�[���`�F���W�X�V
	UpdateChangeScene();
	
}

void GamePlay::Draw()
{
	//�w�i�`��
	objBG.Draw();

	//�X�e�[�W�`��
	stage.Draw();

	//�e�`��
	myBullet.Draw();

	//UI�`��
	DrawUI();

	//���e�N�X�`���`��
	DrawBlackEffect();

	//�`���[�g���A���`��
	tutorial.Draw();

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
		//�ŏ���1�񂾂��X�R�A�^�C�}�[�J�n
		if (isEndFirstEffectOnce == false) {
			scoreTimer.Reset();
			scoreTimer.Start();
		}
		isEndFirstEffectOnce = true;
	}
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
	objBG.AddRotation(0, 0.1f * FPSManager::GetMulAdjust60FPS(), 0);
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
	//ToDo:�}�W�b�N�i���o�[�������牽�Ƃ�����

	//�ړ�
	//�J���G�t�F�N�g�̈ʒu�ړ�(1�񂾂�)
	if (isEndFirstEffectOnce == false) {
		//�u���b�N
		float x = Easing::GetEaseValue(EASE_OUTQUINT, -8.2f, -5.2f, firstEffectTimer, 3500, 4000);
		Vector3 addPos = { x,0,2 };
		objUISquareBlock.SetPosition(camera.GetPosition() + camera.GetCameraDir() * 5 + addPos);
		//���̉���UI
		x = Easing::GetEaseValue(EASE_OUTQUINT, -sprUIRemainingBlock.GetTexSize().x, 8, firstEffectTimer, 3500, 4000);
		sprUIRemainingBlock.SetPosition({ x, sprUIRemainingBlock.GetPosition().y });
		//�c��u���b�N��
		x = Easing::GetEaseValue(EASE_OUTQUINT, -200, 128, firstEffectTimer, 3500, 4000);
		SetRemainingBlockCountTexPos(sprRemainingBlockCount, _countof(sprRemainingBlockCount), { x, 116 }, 40);
		//UI�{�^��
		x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonReset.GetTexSize().x, 10, firstEffectTimer, 3750, 4250);
		buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
		x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonBack.GetTexSize().x, 10, firstEffectTimer, 4000, 4500);
		buttonBack.SetPosition({ x, buttonBack.GetPosition().y });
	}
	//�N���A�G�t�F�N�g�̈ʒu�ړ�
	else if (clearEffectTimer.GetIsStart()) {
		//�u���b�N
		float x = Easing::GetEaseValue(EASE_INBACK, -5.2f, -8.2f, clearEffectTimer, 500, 1000);
		Vector3 addPos = { x,0,2 };
		objUISquareBlock.SetPosition(camera.GetPosition() + camera.GetCameraDir() * 5 + addPos);
		//���̉���UI
		x = Easing::GetEaseValue(EASE_INBACK, 8, -sprUIRemainingBlock.GetTexSize().x, clearEffectTimer, 500, 1000);
		sprUIRemainingBlock.SetPosition({ x, sprUIRemainingBlock.GetPosition().y });
		//�c��u���b�N��
		x = Easing::GetEaseValue(EASE_INBACK, 128, -200, clearEffectTimer, 500, 1000);
		SetRemainingBlockCountTexPos(sprRemainingBlockCount, _countof(sprRemainingBlockCount), { x, 116 }, 40);
		//UI�{�^��
		x = Easing::GetEaseValue(EASE_INBACK, 10, -buttonReset.GetTexSize().x, clearEffectTimer, 750, 1250);
		buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
		x = Easing::GetEaseValue(EASE_INBACK, 10, -buttonBack.GetTexSize().x, clearEffectTimer, 1000, 1500);
		buttonBack.SetPosition({ x, buttonBack.GetPosition().y });
	}

	//�u���b�N��]
	static float add = 0;
	add += 1.0f * FPSManager::GetMulAdjust60FPS();
	if (add >= 360) { add -= 360; }
	objUISquareBlock.SetRotation(camera.GetRotation() + Vector3(add, add, add));
	objUISquareBlock.Update();

	//�c��u���b�N���Z�b�g
	SetRemainingBlockCountTex(sprRemainingBlockCount, _countof(sprRemainingBlockCount), stage.GetTargetBlockCount(), 48, 64);

	//�{�^������
	if (GameUtility::GetIsPause() == false) {
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
			//�X�R�A�^�C�}�[�ꎞ��~
			scoreTimer.Stop();
			//���̓]������ʉ���~
			GameSound::Stop(L"Shooting");
			//�{�^����������悤��
			buttonYes.SetIsEnable(true);
			buttonNo.SetIsEnable(true);
		}
	}


	//�|�[�Y���ƃN���A���̓{�^��������
	bool isClear = stage.GetTargetBlockCount() <= 0;
	buttonReset.SetIsEnable(!isDispPopup && !isClear);
	buttonBack.SetIsEnable(!isDispPopup && !isClear);
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
				//�{�^���������Ȃ��悤��
				buttonYes.SetIsEnable(false);
				buttonNo.SetIsEnable(false);
				//BGM�X�g�b�v(1�b�����ăt�F�[�h�A�E�g)
				GameSound::Stop(L"GamePlay", 1000);
			}
			if (buttonNo.IsReleaseButton()) {
				buttonNo.StartPushedEffect();
				dispPopUpTimer.Reset();
				dispPopUpTimer.Start();
				isDispPopup = false;
				//�{�^���������Ȃ��悤��
				buttonYes.SetIsEnable(false);
				buttonNo.SetIsEnable(false);
			}
		}
		
		//�|�b�v�A�b�v�^�C�}�[�I���Ń|�[�Y����
		if (isDispPopup == false && dispPopUpTimer.GetIsEnd()) {
			GameUtility::SetIsPause(false);
			//�X�R�A�^�C�}�[�ăX�^�[�g
			scoreTimer.Start();
		}
	}
}

void GamePlay::UpdateClearEffect()
{
	//�N���A�G�t�F�N�g���̂ݍX�V
	if (clearEffectTimer.GetNowTime() <= clearEffectTimer.GetStartTime()) {
		return;
	}

	const int START_MOVE_TEXT_TIME = 5000;
	const int END_MOVE_TEXT_TIME = START_MOVE_TEXT_TIME + 250;
	const int START_WHITE_EFFECT = 5750;
	const int START_SCORE_TIMER_EFFECT = 6250;
	const int START_SCORE_TIMER_EFFECT_ROLL_NUMBER = 6750;
	const int START_OK_BUTTON_EFFECT = 8500;

	//�N���A��������ʂ̗��[���璆�S��
	for (int i = 0; i < _countof(sprTextClear); i++) {
		//�e�N�X�`���ړ��J�n�I���ʒu
		float startPosX = 0, endPosX = DX12Util::GetWindowWidth() * 0.5f - sprTextClear[i].GetTexSize().x * 0.5f;
		if (i == 0) { startPosX = -sprTextClear[i].GetTexSize().x; }
		else { startPosX = DX12Util::GetWindowWidth(); }

		Vector2 texPos = { startPosX, 150 };
		texPos.x = Easing::GetEaseValue(EASE_LINE, startPosX, endPosX, clearEffectTimer, START_MOVE_TEXT_TIME, END_MOVE_TEXT_TIME);

		sprTextClear[i].SetPosition(texPos);
	}

	//���e�N�X�`���ƍ��e�N�X�`��
	if (clearEffectTimer.GetNowTime() >= START_WHITE_EFFECT) {
		//���e�N�X�`�������x�X�V
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 0.5f, clearEffectTimer, START_WHITE_EFFECT, START_WHITE_EFFECT + 500);
		Vector4 color = sprBlack.GetColor();
		color.w = alpha;
		sprBlack.SetColor(color);
	}
	else{
		sprBlack.SetColor({ 1,1,1,0 });
	}

	//�X�R�A�^�C�}�[
	if (clearEffectTimer.GetNowTime() >= START_SCORE_TIMER_EFFECT) {
		//�uClearTime�v�����̓����x�X�V
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 1, clearEffectTimer, START_SCORE_TIMER_EFFECT, START_SCORE_TIMER_EFFECT_ROLL_NUMBER);
		Vector4 color = sprTextClearTime.GetColor();
		color.w = alpha;
		sprTextClearTime.SetColor(color);

		//�X�R�A�^�C�������̏���
		SetScoreTimeTex(sprTextTimeNumber, _countof(sprTextTimeNumber), START_SCORE_TIMER_EFFECT_ROLL_NUMBER);

		//�us�v�Ɓu.�v�𔲂���������
		int numberCount = (GetStrScoreTime(-1).size() - 2);
		int endSound = START_SCORE_TIMER_EFFECT_ROLL_NUMBER + numberCount * 250;

		//���ʉ��Đ�
		if (GameSound::IsPlaying(L"RollNumber") == false && 
			clearEffectTimer.GetNowTime() >= START_SCORE_TIMER_EFFECT_ROLL_NUMBER &&
			clearEffectTimer.GetNowTime() < endSound) {
			GameSound::Play(L"RollNumber");
		}
		//���ʉ���~
		if (GameSound::IsPlaying(L"RollNumber") == true && 
			clearEffectTimer.GetNowTime() >= endSound) {
			GameSound::Stop(L"RollNumber");
		}
	}
	else {
		sprTextClearTime.SetColor({ 1,1,1,0 });
		for (int i = 0; i < _countof(sprTextTimeNumber); i++) {
			sprTextTimeNumber[i].SetColor({ 1,1,1,0 });
		}
	}

	//OK�{�^��
	if (clearEffectTimer.GetNowTime() >= START_OK_BUTTON_EFFECT) {
		float adjust = 10;
		float x = Easing::GetEaseValue(EASE_OUTQUINT, DX12Util::GetWindowWidth(), DX12Util::GetWindowWidth() - buttonOK.GetTexSize().x - adjust, clearEffectTimer, START_OK_BUTTON_EFFECT, START_OK_BUTTON_EFFECT + 500);
		buttonOK.SetPosition({ x, buttonOK.GetPosition().y });

		//OK�{�^�������ꂽ
		if (buttonOK.IsReleaseButton()) {
			buttonOK.StartPushedEffect();
			sceneChangeTimer.Start();
			buttonOK.SetIsEnable(false);
			//BGM�X�g�b�v(1�b�����ăt�F�[�h�A�E�g)
			GameSound::Stop(L"GamePlay", 1000);
		}
	}

	//�͂�����ԃp�[�e�B�N���ǉ��ƃN���A���ʉ�
	if (clearEffectTimer.GetNowTime() >= END_MOVE_TEXT_TIME && addedParticleClearEffect == false) {
		for (int i = 0; i < 64; i++) {
			float rad = 360.0f / 32 * i * PI / 180;

			Vector2 pos = Vector2(DX12Util::GetWindowWidth() / 2, 260);
			pos += Vector2(cos(rad) * 2.0f, sin(rad)) * 90;
			Vector2 vel = Vector2(cos(rad) * 2.0f, sin(rad)) * 8 * FPSManager::GetMulAdjust60FPS();
			Vector2 acc = Vector2(0, 0.005f) * FPSManager::GetMulAdjust60FPS();
			particle[0].Add(1000, pos, vel, acc, 0.25f, 0);
		}
		addedParticleClearEffect = true;
		addParticleTimer.Start();

		//�N���A���ʉ�
		GameSound::Play(L"Clear");
	}

	//�L���L������p�[�e�B�N���ǉ�
	if (addParticleTimer.GetIsEnd()) {
		float x = (float)((rand() % 550 - 275));
		float y = (float)((rand() % 300 - 150));

		Vector2 pos = Vector2(DX12Util::GetWindowWidth() / 2, 260) + Vector2(x, y);

		particle[1].Add(1000, pos, {}, {}, 0, 0, 0.1f);
		addParticleTimer.Reset();
		addParticleTimer.Start();
	}


	//�p�[�e�B�N���X�V
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Update();
	}
}

void GamePlay::UpdateChangeScene()
{
	//�V�[���`�F���W�X�V
	if (sceneChangeTimer.GetIsStart()) {
		//���e�N�X�`�������x�X�V
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 1, sceneChangeTimer, 0, 1000);
		//���e�N�X�`�������Ƃ��Ĉ���
		Vector4 color = { 0,0,0,alpha };
		sprWhite.SetColor(color);
	}

	//�^�C�}�[�I���ŃV�[���`�F���W���s
	if (sceneChangeTimer.GetIsEnd()) {
		SceneManager::ChangeScene("StageSelect");
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
	//�{�^���`��
	buttonReset.Draw();
	buttonBack.Draw();
	//�u���b�N�`��
	if (firstEffectTimer.GetNowTime() >= 3500 || isEndFirstEffectOnce) {
		objUISquareBlock.Draw();
	}
	//���̉���UI�`��
	sprUIRemainingBlock.DrawFG();
	//�c��u���b�N���`��
	for (int i = 0; i < _countof(sprRemainingBlockCount); i++) {
		sprRemainingBlockCount[i].DrawFG();
	}
}

void GamePlay::DrawStageBackPopUp()
{
	//�|�[�Y���ɕ`��
	if (GameUtility::GetIsPause()) {
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
	for (int i = 0; i < _countof(sprTextClear); i++) {
		sprTextClear[i].DrawFG();
	}

	//�uClearText�v����
	sprTextClearTime.DrawFG();

	//�X�R�A�^�C������
	for (int i = 0; i < _countof(sprTextTimeNumber); i++) {
		sprTextTimeNumber[i].DrawFG();
	}

	//OK�{�^��
	buttonOK.Draw();

	//�p�[�e�B�N��
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Draw();
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
	firstEffectTimer.SetTimer(MYBULLET_START_FIRST_EFFECT_TIME, 4500);
	firstEffectTimer.Start();

	//�N���A�G�t�F�N�g�^�C�}�[���Z�b�g
	clearEffectTimer.Reset();

	//�e������
	myBullet.Initialize();

	//�O�̂��߃J�������ʒu��
	camera.SetCameraParamAfterShoot();
	camera.Update();

	//�O�̂��ߔ��ƍ��e�N�X�`�������x0��
	sprWhite.SetColor({ 1,1,1,0 });
	sprBlack.SetColor({ 1,1,1,0 });

	//�X�e�[�W�J���[������
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//UI���W���ʒu��
	//�u���b�N
	Vector3 addPos = { -5.2f,0,2 };
	objUISquareBlock.SetPosition(camera.GetPosition() + camera.GetCameraDir() * 5 + addPos);
	//���̉���UI
	sprUIRemainingBlock.SetPosition({ 8, sprUIRemainingBlock.GetPosition().y });
	//�c��u���b�N��
	SetRemainingBlockCountTexPos(sprRemainingBlockCount, _countof(sprRemainingBlockCount), { 128, 116 }, 40);
	//UI�{�^��
	buttonReset.SetPosition({ 10, buttonReset.GetPosition().y });
	buttonBack.SetPosition({ 10, buttonBack.GetPosition().y });
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
		//�X�R�A�^�C�}�[��~
		scoreTimer.Stop();

		//���̓]������ʉ���~
		GameSound::Stop(L"Shooting");
	}
}

void GamePlay::SetRemainingBlockCountTex(Sprite* pNumberTexArray, int arraySize, int drawNum, float numWidth, float numHeight)
{
	//�`�悷�镶����
	std::string drawStr = std::to_string(drawNum);

	//�����`�F�b�N
	//�����I�[�o�[��999����
	if (drawStr.size() > arraySize) {
		drawStr = "";
		for (int i = 0; i < arraySize; i++) {
			drawStr += "9";
		}
	}
	else {
		//������
		while (drawStr.size() < arraySize) {
			drawStr = "#" + drawStr;
		}
	}

	for (int i = 0; i < arraySize; i++) {
		//�`�悷��1����
		const std::string draw = drawStr.substr(i, 1);

		//�`�悵�Ȃ�
		if (draw == "#") {
			pNumberTexArray[i].SetColor({ 1,1,1,0 });
		}
		//����
		else {
			pNumberTexArray[i].SetColor({ 1,1,1,1 });
			pNumberTexArray[i].SetDrawRectangle(stoi(draw) * numWidth, 0, numWidth, numHeight);
		}
	}
}

void GamePlay::SetRemainingBlockCountTexPos(Sprite* pNumberTexArray, int arraySize, const Vector2& leftUpPos, float padding)
{
	for (int i = 0; i < arraySize; i++) {
		pNumberTexArray[i].SetPosition(leftUpPos + Vector2(padding * i, 0));
	}
}

void GamePlay::SetScoreTimeTex(Sprite* pNumberTexArray, int arraySize, int startDrawTime)
{
	//�����e�N�X�`��1���̑傫��
	const Vector2 ONE_TEX_SIZE = { 72,96 };
	//�X�R�A�^�C���̍�����W
	const Vector2 NUMBER_MASTER_POS = { 575,450 };
	//�������炷�p
	float drawPosX = 0;
	//����������Ƃ��̐��l���E�ɂ���ĕ`�悳���
	float padding = 60;
	//�`�悷�镶����
	const std::string DRAW_STR = GetStrScoreTime(arraySize);

	//�`��J�n�ʒu�is�͍ŏ�����`�悷��j
	int start = arraySize;
	if (clearEffectTimer.GetNowTime() >= startDrawTime) {
		start = arraySize - 1 - ((clearEffectTimer.GetNowTime() - startDrawTime) * 0.004f);

		//�����_�͉��o���΂�
		if (start <= arraySize - 5) {
			start--;
		}
	}

	for (int i = 0; i < arraySize; i++) {
		//�`�悷��1����
		const std::string draw = DRAW_STR.substr(i,1);

		pNumberTexArray[i].SetPosition(NUMBER_MASTER_POS + Vector2{ drawPosX, 0 });
		pNumberTexArray[i].SetColor({ 1,1,1,1 });

		//�`�悵�Ȃ�����
		if (draw == "#") {
			pNumberTexArray[i].SetDrawRectangle(0, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
			pNumberTexArray[i].SetColor({ 1,1,1,0 });
		}
		//�����_
		else if (draw == ".") {
			//�ʒu����
			padding = 20;
			pNumberTexArray[i].SetPosition(NUMBER_MASTER_POS + Vector2{ drawPosX - 20, 0 });
			pNumberTexArray[i].SetDrawRectangle(ONE_TEX_SIZE.x * 11, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
		}
		//�us�v
		else if (draw == "s") {
			pNumberTexArray[i].SetDrawRectangle(ONE_TEX_SIZE.x * 10, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
		}
		//��������
		else {
			int drawNum = stoi(draw);
			if (i == start) {
				drawNum = rand() % 10;
			}

			pNumberTexArray[i].SetDrawRectangle(drawNum * ONE_TEX_SIZE.x, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
		}

		//�`��ʒu���O�̐����͕`�悵�Ȃ�
		if (i < start) {
			pNumberTexArray[i].SetColor({ 1,1,1,0 });
		}

		drawPosX += padding;
		padding = 60;
	}
}

std::string GamePlay::GetStrScoreTime(int keta)
{
	std::string str = std::to_string(scoreTimer.GetNowTime());

	str = str.insert(str.size() - 3, ".");
	str = str.insert(str.size(), "s");

	//keta��-1����Ȃ��Ƃ��A������keta�Ɠ��������ɂȂ�悤��#�Ŗ��߂�
	if (keta != -1) {
		while (str.size() < keta) {
			str = "#" + str;
		}
	}

	return str;
}
