#include "StageSelect.h"
#include "Input.h"
#include "GameUtility.h"
#include "Easing.h"
#include "DebugText.h"
#include "SceneManager.h"

const std::string StageSelect::STAGE_DIRECTORY = "StageData/";

StageSelect::StageSelect()
{
	//�X�e�[�W���[�h
	for (int i = 0; i < STAGE_COUNT; i++) {
		stages.emplace_back(new Stage());
		stages[i]->LoadStage(STAGE_DIRECTORY + "stage_" + std::to_string(i) + ".spb");
		stages[i]->Initialize();
		stages[i]->SetMasterPosition(Vector3(0, 0, i * 500));

	}

	//UI�摜�ǂݍ���
	buttonUp.LoadTexture(L"Resources/UI/UI_Arrow_Up.png");
	buttonDown.LoadTexture(L"Resources/UI/UI_Arrow_Down.png");
	buttonStart.LoadTexture(L"Resources/UI/UI_Start.png");

	//�X�v���C�g�ǂݍ���
	sprStageBG.Initialize();
	sprStageBG.SetTexture(L"Resources/Stage_BG.png");
	sprBackground.Initialize();
	sprBackground.SetTexture(L"Resources/Background.png");
	sprTextStage.Initialize();
	sprTextStage.SetTexture(L"Resources/Text_Stage.png");
	for (int i = 0; i < _countof(sprStageNum); i++) {
		sprStageNum[i].Initialize();
		sprStageNum[i].SetTexture(L"Resources/UI/UI_Number.png");
	}
	sprWrite.Initialize();
	sprWrite.SetTexture(L"Resources/White1280x720.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(L"Resources/Black1280x720.png");
}

StageSelect::~StageSelect()
{
	//���
	for (int i = 0; i < stages.size(); i++) {
		delete stages[i];
		stages[i] = nullptr;
	}
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

	//�X�e�[�W������

	//UI�{�^��������
	float adjust = 10;
	buttonUp.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - buttonUp.GetTexSize().y - adjust * 2 });
	buttonDown.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - adjust });
	buttonStart.Initialize({ DX12Util::GetWindowWidth() - buttonStart.GetTexSize().x - adjust, DX12Util::GetWindowHeight() - buttonStart.GetTexSize().y - adjust });

	//�X�v���C�g������
	sprBlack.SetColor({ 1,1,1,1 });
	sprTextStage.SetPosition({ DX12Util::GetWindowWidth() - sprTextStage.GetTexSize().x, 0 });

	//�^�C�}�[�Z�b�g
	firstEffectTimer.SetTimer(0, 250);
	firstEffectTimer.Start();
	startGameTimer.SetTimer(0, 2000);
	changeSelectPosTimer.SetTimer(0, 1000);
	changeSelectPosTimer.SetNowTime(changeSelectPosTimer.GetEndTime());
	roopEffectTimer.SetTimer(0, 4000, true);
	roopEffectTimer.Start();
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

	UpdateStage();

	UpdateStageNumTex();
}

void StageSelect::Draw()
{
	DrawStage();
	DrawUI();
	DrawFG();
}

void StageSelect::UpdateCamera()
{
	//�^�C�}�[�X�V
	changeSelectPosTimer.Update();

	if (startGameTimer.GetIsStart() == false) {
		if (changeSelectPosTimer.GetIsStart()) {
			if (isMoveUp) {
				Vector3 setPos = camera.GetPosition();
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex - 1) * 500 - 50, nowSelectStageIndex * 500 - 50, changeSelectPosTimer);
				camera.SetPositionAndDistance(setPos, 15.0f);
				light.SetLightTarget({ 0,0,setPos.z + 50 });
				light.CalcLightPos(100.0f);
			}
			else {
				Vector3 setPos = camera.GetPosition();
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex + 1) * 500 - 50, nowSelectStageIndex * 500 - 50, changeSelectPosTimer);
				camera.SetPositionAndDistance(setPos, 15.0f);
				light.SetLightTarget({ 0,0,setPos.z + 50 });
				light.CalcLightPos(100.0f);
			}
		}
		else {
			Vector3 setPos = { 0,150,(float)nowSelectStageIndex * 500 - 50 };
			camera.SetPositionAndDistance(setPos, 15.0f);
			light.SetLightTarget({ 0,0,setPos.z + 50 });
			light.CalcLightPos(100.0f);
		}
	}
	else {
		Vector3 setPos = { 0,150,(float)nowSelectStageIndex * 500 - 50 };
		float addY = Easing::GetEaseValue(EASE_INBACK, 0, -150, startGameTimer);
		float addZ = Easing::GetEaseValue(EASE_INBACK, 0, 50, startGameTimer);

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
	}
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
		GameUtility::SetNowStagePath(STAGE_DIRECTORY + "stage_" + std::to_string(nowSelectStageIndex) + ".spb");
		//�V�[�����ڂ�
		SceneManager::ChangeScene("GamePlay");
	}
}

void StageSelect::UpdateStageNumTex()
{
	std::string drawStr = std::to_string(nowSelectStageIndex);
	int arraySize = _countof(sprStageNum);
	float numWidth = 48, numHeight = 64;
	Vector2 masterPos = { 1185,5 };
	float padding = 35;
	float addPosY = Easing::GetEaseValue(EASE_OUTQUINT, masterPos.y - 10, masterPos.y, changeSelectPosTimer, 0, 500);

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

void StageSelect::DrawStage()
{
	sprBackground.DrawBG();
	sprStageBG.DrawBG();

	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Draw();
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
