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
	buttonReset.LoadTexture(L"Resources/UI/UI_Arrow_Reset.png");
	buttonBack.LoadTexture(L"Resources/UI/UI_Arrow_Back.png");
}

GamePlay::~GamePlay()
{
}

void GamePlay::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//フェーズセット
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//カメラ初期化
	camera.Initialize();
	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(80.0f);
	//ライトをセット
	Object3D::SetLight(&light);

	//ステージ取得
	stage.Initialize();
	stage.LoadStage(GameUtility::GetNowStagePath());

	//ステージサイズからカメラ位置セット
	float bounceY = camera.SetPosFromStageSize(stage.GetStageSize());

	//弾初期化
	myBullet.SetPStage(&stage);
	myBullet.Initialize();
	myBullet.SetBounceInitPosY(bounceY);

	//背景初期化
	modelBG.CreateFromOBJ(modelDir + "Sky/Sky.obj");
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	sprWhite.Initialize();
	sprWhite.SetTexture(L"Resources/Write1280x720.png");

	//タイマー初期化
	startEffectTimer.SetTimer(0, 4250);
	startEffectTimer.Start();

	//UIボタン初期化
	float adjust = 10;
	buttonReset.Initialize({ -buttonReset.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - buttonReset.GetTexSize().y - adjust * 2 });
	buttonBack.Initialize({ -buttonBack.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - adjust });

	//ステージカラー初期化
	GameUtility::SetStageColor(STAGE_COLOR_NONE);
}

void GamePlay::Update()
{
	UpdateDebugCamera();
	camera.Update();
	light.Update();
	//3Dサウンドで使用するリスナーの位置更新
	Sound::Set3DListenerPosAndVec(camera);
	UpdateImgui();
	//UI更新
	UpdateUI();
	//クリアしているか？
	CheckIsClear();
	//背景オブジェクト更新
	UpdateBG();
	//弾更新
	myBullet.Update();
	//ステージ更新
	stage.Update();
	//エフェクト更新
	UpdateEffect();
}

void GamePlay::Draw()
{
	//背景描画
	objBG.Draw();

	//弾描画
	myBullet.Draw();

	//ステージ描画
	stage.Draw();

	//エフェクト描画
	DrawEffect();

	//UI描画
	DrawUI();
}

void GamePlay::UpdateDebugCamera()
{
#ifdef _DEBUG
	//カメラ移動
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

	//カメラ回転
	if (Keyboard::IsKeyPush(DIK_UP)) {
		Vector3 f = { camera.GetRotation().x - 1.5f, camera.GetRotation().y, camera.GetRotation().z };
		camera.SetRotation(f);
	}
	if (Keyboard::IsKeyPush(DIK_DOWN)) {
		Vector3 f = { camera.GetRotation().x + 1.5f, camera.GetRotation().y, camera.GetRotation().z };
		camera.SetRotation(f);
	}
	if (Keyboard::IsKeyPush(DIK_LEFT)) {
		//Shiftが押されていたらZ軸回転
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

void GamePlay::UpdateEffect()
{
	//完全に透明でなければ更新
	if (startEffectTimer.GetIsEnd() == false) {
		startEffectTimer.Update();
		float alpha = Easing::GetEaseValue(EASE_INOUTEXPO, 1, 0, startEffectTimer, 0, 500);
		Vector4 color = sprWhite.GetColor();
		color.w = alpha;
		sprWhite.SetColor(color);
	}
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
	//開幕の位置移動
	float x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonReset.GetTexSize().x, 10, startEffectTimer, 3500, 4000);
	buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
	x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonBack.GetTexSize().x, 10, startEffectTimer, 3750, 4250);
	buttonBack.SetPosition({ x, buttonBack.GetPosition().y });

	//ボタン入力
	if (buttonReset.IsReleaseButton()) {
		Reset();
	}
	if (buttonBack.IsReleaseButton()) {
		//Todo:確認ポップアップ作る
		SceneManager::ChangeScene("StageSelect");
	}

}

void GamePlay::DrawEffect()
{
	//完全に透明でなければ描画
	if (startEffectTimer.GetIsEnd() == false) {
		sprWhite.DrawFG();
	}
}

void GamePlay::DrawUI()
{
	buttonReset.Draw();
	buttonBack.Draw();
}

void GamePlay::Reset()
{
	//開幕エフェクト中はリセットさせない
	if (GameUtility::GetNowPhase() == PHASE_GAME_FIRSTEFFECT) {
		return;
	}

	//if (Keyboard::IsKeyTrigger(DIK_R)) {
	//フェーズ初期化
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//ステージ取得
	stage.LoadStage(GameUtility::GetNowStagePath());

	//弾初期化
	myBullet.Initialize(false);

	//念のためカメラを定位置に
	camera.SetCameraParamAfterShoot();

	//ステージカラー初期化
	GameUtility::SetStageColor(STAGE_COLOR_NONE);
	//}

	//デバッグ用　完成版は消す
	//if (Keyboard::IsKeyTrigger(DIK_Q)) {

	//	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//	//ステージ取得
	//	stage.Initialize();
	//	stage.LoadStage(GameUtility::GetNowStagePath());

	//	//弾初期化
	//	myBullet.Initialize();

	//	//カメラ初期化　完成時はずす
	//	camera.Initialize();

	//	//ステージサイズからカメラ位置セット
	//	camera.SetPosFromStageSize(stage.GetStageSize());

	//	//ステージカラー初期化
	//	GameUtility::SetStageColor(STAGE_COLOR_NONE);
	//}
}

void GamePlay::CheckIsClear()
{
	//残り目標破壊ブロックが0だったらクリア
	if (GameUtility::GetNowPhase() == PHASE_GAME_AFTERSHOOT && 
		stage.GetTargetBlockCount() <= 0) {
		GameUtility::SetNowPhase(PHASE_GAME_CLEAR);
		//各種クリアエフェクトタイマースタート
		stage.StartEffectTimer(0, 10000);
		camera.StartEffectTimer(0, 5000);
	}
}
