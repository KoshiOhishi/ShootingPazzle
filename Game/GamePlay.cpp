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

	//カメラ初期化
	camera.Initialize();
	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 1,-1,1 });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(100.0f);
	//ライトをセット
	Object3D::SetLight(&light);

	//フェーズセット
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//ステージ取得
	stage.Initialize();
	stage.LoadStage(stagePass);

	//ステージサイズからカメラ位置セット
	float bounceY = camera.SetPosFromStageSize(stage.GetStageSize());

	//弾初期化
	myBullet.SetPStage(&stage);
	myBullet.Initialize();
	myBullet.SetBounceInitPosY(bounceY);

	//背景初期化
	modelBG.CreateFromOBJ("sky");
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	//ステージカラー初期化
	GameUtility::SetStageColor(STAGE_COLOR_NONE);
}

void GamePlay::Update()
{
	// DirectX 毎フレーム処理 ここから

	//入力処理ここから

#pragma region カメラ
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

#pragma endregion
#pragma region サウンド

#pragma endregion
#pragma region オブジェクト

	objBG.AddRotation(0, 0.1f, 0);

	//リセット
	Reset();

	//クリアしているか？
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
#pragma region アップデート
	camera.Update();
	light.Update();
	objBG.Update();
	//3Dサウンドで使用するリスナーの位置更新
	Sound::Set3DListenerPosAndVec(camera);

	//弾更新
	myBullet.Update();
	//ステージ更新
	stage.Update();

#pragma endregion
}

void GamePlay::Draw()
{
	//背景描画
	objBG.Draw();

	//ステージ描画
	stage.Draw();

	//弾描画
	myBullet.Draw();
}

void GamePlay::Reset()
{
	if (Keyboard::IsKeyTrigger(DIK_R)) {
		//ステージ取得
		stage.LoadStage(stagePass);

		//弾初期化
		myBullet.Initialize();

		//念のためカメラを定位置に
		camera.SetCameraParamAfterShoot();

		//フェーズ初期化
		GameUtility::SetNowPhase(PHASE_GAME_SETPOS);

		//ステージカラー初期化
		GameUtility::SetStageColor(STAGE_COLOR_NONE);
	}

	//デバッグ用　完成版は消す
	if (Keyboard::IsKeyTrigger(DIK_Q)) {

		GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

		//ステージ取得
		stage.LoadStage(stagePass);

		//弾初期化
		myBullet.Initialize();

		//カメラ初期化　完成時はずす
		camera.Initialize();

		//ステージサイズからカメラ位置セット
		camera.SetPosFromStageSize(stage.GetStageSize());

		//ステージカラー初期化
		GameUtility::SetStageColor(STAGE_COLOR_NONE);
	}
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
