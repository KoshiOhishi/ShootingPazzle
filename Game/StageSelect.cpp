#include "StageSelect.h"
#include "Input.h"
#include "GameUtility.h"

const std::string StageSelect::STAGE_DIRECTORY = "StageData/";

StageSelect::StageSelect()
{
	//ステージロード
	for (int i = 0; i < STAGE_COUNT; i++) {
		stages.emplace_back(new Stage());
		stages[i]->LoadStage(STAGE_DIRECTORY + "stage_" + std::to_string(i + 1) + ".spb");
	}
}

StageSelect::~StageSelect()
{
	//解放
	for (int i = 0; i < stages.size(); i++) {
		delete stages[i];
		stages[i] = nullptr;
	}
}

void StageSelect::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.15f, 720 * 0.15f, 0.1f, 150.0f);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(75, 0, 0);
	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(100.0f);
	//ライトをセット
	Object3D::SetLight(&light);

	//フェーズセット
	GameUtility::SetNowPhase(PHASE_STAGESELECT_SELECT);

	//選択インデックス初期化
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
