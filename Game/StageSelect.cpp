#include "StageSelect.h"
#include "Input.h"
#include "GameUtility.h"

const std::string StageSelect::STAGE_DIRECTORY = "StageData/";

StageSelect::StageSelect()
{
	//�X�e�[�W���[�h
	for (int i = 0; i < STAGE_COUNT; i++) {
		stages.emplace_back(new Stage());
		stages[i]->LoadStage(STAGE_DIRECTORY + "stage_" + std::to_string(i + 1) + ".spb");
	}
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
	Object3D::SetMatrixOrthographicLH(1280 * 0.15f, 720 * 0.15f, 0.1f, 150.0f);

	//�J����������
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(75, 0, 0);
	//�J�������Z�b�g
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//���C�g������
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(100.0f);
	//���C�g���Z�b�g
	Object3D::SetLight(&light);

	//�t�F�[�Y�Z�b�g
	GameUtility::SetNowPhase(PHASE_STAGESELECT_SELECT);

	//�I���C���f�b�N�X������
	nowSelectStageIndex = 0;

	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Initialize();
	}
}

void StageSelect::Update()
{
	camera.Update();
	light.Update();

	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Update();
	}
}

void StageSelect::Draw()
{
	DrawStageData();
}

void StageSelect::DrawStageData()
{
	//for (int i = 0; i < stages.size(); i++) {
	stages[0]->Draw();
	//}
}
