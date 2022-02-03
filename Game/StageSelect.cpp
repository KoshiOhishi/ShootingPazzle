#include "StageSelect.h"
#include "Input.h"
#include "GameUtility.h"
#include "Easing.h"
#include "DebugText.h"
#include "SceneManager.h"
#include "GameSound.h"
#include "ImguiHelper.h"

StageSelect::StageSelect()
{
	//�X�e�[�W���[�h
	for (int i = 0; i < STAGE_COUNT; i++) {
		stages.emplace_back(new Stage());
		stages[i]->LoadStage(STAGE_DIR + "stage_" + std::to_string(i) + ".spb");
		stages[i]->Initialize();
		stages[i]->SetMasterPosition(Vector3(0, 0, i * 500));
		//�X�e�[�W�̑傫���ɂ���ăJ�����̈ʒu��ς���
		cameraPosList.emplace_back(Vector3(0, stages[i]->GetStageSize().y * 10, i * 500 - stages[i]->GetStageSize().y * 3 - 5));

		//�X�e�[�W�Z���N�g�ł͉e�`�斳���ɂ���
		stages[i]->SetDrawShadow(false);
	}
	//UI�摜�ǂݍ���
	buttonUp.LoadTexture(TEX_DIR_STAGESELECT + L"UI_Arrow_Up.png");
	buttonDown.LoadTexture(TEX_DIR_STAGESELECT + L"UI_Arrow_Down.png");
	buttonStart.LoadTexture(TEX_DIR_STAGESELECT + L"UI_Start.png");

	//�X�v���C�g�ǂݍ���
	sprStageBG.Initialize();
	sprStageBG.SetTexture(TEX_DIR_STAGESELECT + L"Stage_BG.png");
	sprBackground.Initialize();
	sprBackground.SetTexture(TEX_DIR_STAGESELECT + L"Background.png");
	sprTextStage.Initialize();
	sprTextStage.SetTexture(TEX_DIR_STAGESELECT + L"Text_Stage.png");
	sprArrowUp.Initialize();
	sprArrowUp.SetTexture(TEX_DIR_STAGESELECT + L"Arrow_Up.png");
	sprArrowDown.Initialize();
	sprArrowDown.SetTexture(TEX_DIR_STAGESELECT + L"Arrow_Down.png");
	for (int i = 0; i < _countof(sprStageNum); i++) {
		sprStageNum[i].Initialize();
		sprStageNum[i].SetTexture(TEX_DIR_STAGESELECT + L"UI_Number.png");
	}
	sprWrite.Initialize();
	sprWrite.SetTexture(TEX_DIR_UTIL + L"White1280x720.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(TEX_DIR_UTIL + L"Black1280x720.png");

}

StageSelect::~StageSelect()
{
	//���
	for (int i = 0; i < stages.size(); i++) {
		delete stages[i];
		stages[i] = nullptr;
	}
	cameraPosList.clear();
}

void StageSelect::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//�J����������
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(75, 0, 0);
	//�J�������Z�b�g
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);
	Particle3D::SetCamera(&camera);

	//���C�g������
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(80.0f);
	//���C�g���Z�b�g
	Object3D::SetLight(&light);

	//�t�F�[�Y�Z�b�g
	GameUtility::SetNowPhase(PHASE_STAGESELECT_SELECT);

	//UI�{�^��������
	float adjust = 10;
	buttonUp.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - buttonUp.GetTexSize().y - adjust * 2 });
	buttonDown.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - adjust });
	buttonStart.Initialize({ DX12Util::GetWindowWidth() - buttonStart.GetTexSize().x - adjust, DX12Util::GetWindowHeight() - buttonStart.GetTexSize().y - adjust });

	//�X�v���C�g������
	sprBlack.SetColor({ 1,1,1,1 });
	sprTextStage.SetPosition({ 0, 50 });

	//�^�C�}�[�Z�b�g
	firstEffectTimer.SetTimer(0, 250);
	firstEffectTimer.Start();
	startGameTimer.SetTimer(0, 2000);
	changeSelectPosTimer.SetTimer(0, 1000);
	changeSelectPosTimer.SetNowTime(changeSelectPosTimer.GetEndTime());
	roopEffectTimer.SetTimer(0, 4000, true);
	roopEffectTimer.Start();
	arrowTimer.SetTimer(0, 1500, true);
	arrowTimer.Start();

	//�X�e�[�W�J���[������
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//BGM���ĂȂ�������Đ�
	if (GameSound::IsPlaying(L"Title") == false) {
		GameSound::Play(L"Title");
	}
}

void StageSelect::Update()
{
	light.Update();
	UpdateCamera();
	UpdateTimer();

	//�X�e�[�W����O����
	if (startGameTimer.GetIsStart() == false) {
		UpdateNowSelect();
	}
	//�X�e�[�W����㏈��
	UpdateAfterDecided();

	//�X�e�[�W�X�V
	UpdateStage();

	//�X�e�[�W�ԍ��e�N�X�`���X�V
	UpdateStageNumTex();

	//�{�^���X�V
	UpdateButton();

	//�T�E���h�X�V
	GameSound::Update();
}

void StageSelect::Draw()
{
	//�X�e�[�W�`��
	DrawStage();
	//UI�`��
	DrawUI();
	//���e�N�X�`���G�t�F�N�g�`��
	DrawFG();
}

void StageSelect::UpdateCamera()
{
	//�^�C�}�[�X�V
	changeSelectPosTimer.Update();

	if (startGameTimer.GetIsStart() == false) {
		float add = 5, m = 4.5;
		Vector3 setPos = {};
		float lightTargetZ = 0, lightDistance = 0, mul = 0, farZ = 0;
		//�p�����[�^�Z�b�g
		if (changeSelectPosTimer.GetIsStart()) {
			if (isMoveUp) {
				//�J�����ʒu
				setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex - 1].y, cameraPosList[nowSelectStageIndex].y, changeSelectPosTimer);
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex - 1].z, cameraPosList[nowSelectStageIndex].z, changeSelectPosTimer);
				//���C�g�ʒu
				lightTargetZ = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex - 1) * 500, nowSelectStageIndex * 500, changeSelectPosTimer);
				lightDistance = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex - 1]->GetStageSize().y * 2.5,
					stages[nowSelectStageIndex]->GetStageSize().y * 2.5, changeSelectPosTimer);
				//���C�g�Ɋւ���s��X�V
				mul = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex - 1]->GetStageSize().y * 0.012,
					stages[nowSelectStageIndex]->GetStageSize().y * 0.012, changeSelectPosTimer);
				farZ = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex - 1]->GetStageSize().y * m + add,
					stages[nowSelectStageIndex]->GetStageSize().y * m + add, changeSelectPosTimer);
			}
			else {
				//�J�����ʒu
				setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex + 1].y, cameraPosList[nowSelectStageIndex].y, changeSelectPosTimer);
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex + 1].z, cameraPosList[nowSelectStageIndex].z, changeSelectPosTimer);
				//���C�g�ʒu
				lightTargetZ = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex + 1) * 500, nowSelectStageIndex * 500, changeSelectPosTimer);
				lightDistance = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex + 1]->GetStageSize().y * 2.5, 
					stages[nowSelectStageIndex]->GetStageSize().y * 2.5, changeSelectPosTimer);
				//���C�g�Ɋւ���s��X�V
				mul = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex + 1]->GetStageSize().y * 0.012,
					stages[nowSelectStageIndex]->GetStageSize().y * 0.012, changeSelectPosTimer);
				farZ = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex + 1]->GetStageSize().y * m + add,
					stages[nowSelectStageIndex]->GetStageSize().y * m + add, changeSelectPosTimer);
			}
		}
		else {
			//�J�����ʒu
			setPos = cameraPosList[nowSelectStageIndex];
			camera.SetPositionAndDistance(setPos, 15.0f);
			//���C�g�ʒu
			lightTargetZ = nowSelectStageIndex * 500;
			lightDistance = stages[nowSelectStageIndex]->GetStageSize().y * 2.5;
			//���C�g�Ɋւ���s��X�V
			mul = stages[nowSelectStageIndex]->GetStageSize().y * 0.012;
			farZ = stages[nowSelectStageIndex]->GetStageSize().y * m + add;
		}
		//���f
		camera.SetPositionAndDistance(setPos, 15.0f);
		light.SetLightTarget({ 0,0, lightTargetZ });
		light.CalcLightPos(lightDistance);
		Object3D::SetMatrixOrthographicLH(1280 * mul, 720 * mul, 0.01f, farZ);
	}
	else {
		Vector3 setPos = cameraPosList[nowSelectStageIndex];
		float addY = Easing::GetEaseValue(EASE_INBACK, 0, -cameraPosList[nowSelectStageIndex].y, startGameTimer);
		float addZ = Easing::GetEaseValue(EASE_INBACK, 0, -cameraPosList[nowSelectStageIndex].z + nowSelectStageIndex * 500, startGameTimer);

		setPos += {0, addY, addZ};

		camera.SetPositionAndDistance(setPos, 15.0f);
	}
	camera.Update();
}

void StageSelect::UpdateTimer()
{
	firstEffectTimer.Update();
	startGameTimer.Update();
	roopEffectTimer.Update();
	arrowTimer.Update();
	//�ړ����͖�������(�^�C�}�[���ŏI�l��)
	if (changeSelectPosTimer.GetIsStart()) {
		arrowTimer.SetNowTime(arrowTimer.GetEndTime());
	}
}

void StageSelect::UpdateNowSelect()
{
	//�J�[�\�������
	if (buttonUp.IsReleaseButton()) {
		buttonUp.StartPushedEffect();
		nowSelectStageIndex++;
		if (nowSelectStageIndex >= STAGE_COUNT) { 
			nowSelectStageIndex = STAGE_COUNT - 1;
		}
		else {
			//�^�C�}�[�X�^�[�g
			changeSelectPosTimer.SetTimer(0, 1000);
			changeSelectPosTimer.Start();
			//������ړ�
			isMoveUp = true;
		}
	}

	//�J�[�\��������
	if (buttonDown.IsReleaseButton()) {
		buttonDown.StartPushedEffect();
		nowSelectStageIndex--;
		if (nowSelectStageIndex < 0) {
			nowSelectStageIndex = 0; 
		}
		else {
			//�^�C�}�[�X�^�[�g
			changeSelectPosTimer.SetTimer(0, 1000);
			changeSelectPosTimer.Start();
			//�������ړ�
			isMoveUp = false;
		}
	}

	//�I�����ꂽ�X�e�[�W�ŃQ�[���X�^�[�g
	if (buttonStart.IsReleaseButton()) {
		buttonStart.StartPushedEffect();
		startGameTimer.Start();
		//BGM�X�g�b�v(1�b�����ăt�F�[�h�A�E�g)
		GameSound::Stop(L"Title", 1000);

		//���ʉ��Đ�
		GameSound::Play(L"StageDecide");
	}

	//���
	
	//�ʒu
	//��
	Vector2 setPos = { (float)DX12Util::GetWindowWidth() / 2 - sprArrowUp.GetTexSize().x / 2, 0 };//���͒��S
	float start = 80, end = 20;
	setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, start, end, arrowTimer);
	sprArrowUp.SetPosition(setPos);
	//��
	float down = (float)DX12Util::GetWindowHeight() - sprArrowUp.GetTexSize().y;
	setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, down - start, down - end, arrowTimer);
	sprArrowDown.SetPosition(setPos);

	//�����x
	Vector4 color = { 1,1,1,1 };
	color.w = Easing::GetEaseValue(EASE_INQUINT, 1, 0, arrowTimer);
	sprArrowUp.SetColor(color);
	sprArrowDown.SetColor(color);
}

void StageSelect::UpdateStage()
{
	//�����ۂ̓����x
	float digrees = (float)roopEffectTimer.GetNowTime() * 180 / roopEffectTimer.GetEndTime();
	float alpha = sin(digrees * PI / 180) * 0.5f + 0.5f;
	sprStageBG.SetColor({ 1,1,1,alpha });

	//�X�e�[�W�X�V
	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Update();
	}
}

void StageSelect::UpdateAfterDecided()
{
	//�G�t�F�N�g�I����ɃQ�[���v���C�V�[����
	if (startGameTimer.GetIsEnd())
	{
		//�X�e�[�W�Z�b�g
		GameUtility::SetNowStagePath(STAGE_DIR + "stage_" + std::to_string(nowSelectStageIndex) + ".spb");
		//�V�[�����ڂ�
		SceneManager::ChangeScene("GamePlay");
	}
}

void StageSelect::UpdateStageNumTex()
{
	std::string drawStr = std::to_string(nowSelectStageIndex);
	int arraySize = _countof(sprStageNum);
	float numWidth = 48, numHeight = 64;
	Vector2 masterPos = { 185,60 };
	float padding = 35;
	float addPosY = Easing::GetEaseValue(EASE_OUTQUINT, -10, 0, changeSelectPosTimer, 0, 500);

	//�����`�F�b�N
	//�����I�[�o�[��99����
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
			sprStageNum[i].SetColor({ 1,1,1,0 });
		}
		//����
		else {
			sprStageNum[i].SetColor({ 1,1,1,1 });
			sprStageNum[i].SetDrawRectangle(stoi(draw) * numWidth, 0, numWidth, numHeight);
		}

		sprStageNum[i].SetPosition(masterPos + Vector2(padding * i, addPosY));
	}

}

void StageSelect::UpdateButton()
{
	//�{�^���L���ݒ�
	//startGameTimer�J�n�O�͗L��
	buttonUp.SetIsEnable(startGameTimer.GetNowTime() <= 0);
	buttonDown.SetIsEnable(startGameTimer.GetNowTime() <= 0);
	buttonStart.SetIsEnable(startGameTimer.GetNowTime() <= 0);
}

void StageSelect::DrawStage()
{
	//�w�i
	sprBackground.DrawBG();
	//�����~
	sprStageBG.DrawBG();
	//�X�e�[�W�{��
	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Draw();
	}
	//���
	//�Q�[���J�n�^�C�}�[�J�n�O�Ȃ�`��
	if (startGameTimer.GetIsStart() == false) {
		//��
		if (nowSelectStageIndex < stages.size() - 1) {
			sprArrowUp.DrawFG();
		}
		//��
		if (nowSelectStageIndex > 0) {
			sprArrowDown.DrawFG();
		}
	}
}

void StageSelect::DrawUI()
{
	buttonUp.Draw();
	buttonDown.Draw();
	buttonStart.Draw();
	sprTextStage.DrawFG();
	DrawStageNumTex();
}

void StageSelect::DrawFG()
{
	//��
	float alpha = 1.0f - ((float)firstEffectTimer.GetNowTime() / firstEffectTimer.GetEndTime());
	sprBlack.SetColor({ 1,1,1,alpha });
	sprBlack.DrawFG();

	//��
	alpha = Easing::GetEaseValue(EASE_INQUART, 0, 1, startGameTimer);
	sprWrite.SetColor({ 1, 1, 1, alpha });
	sprWrite.DrawFG();
}

void StageSelect::DrawStageNumTex()
{
	for (int i = 0; i < _countof(sprStageNum); i++) {
		sprStageNum[i].DrawFG();
	}
}