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
	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 1,1,1 });
	light.SetLightColor({ 1,1,1 });
	Object3D::SetLight(&light);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,50.0f,-70.0f }, 15.0f);
	camera.SetRotation(45, 0, 0);

	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//タイマーセット
	timer.SetTimer(0, 1000000);
	timer.Start();

	//ステージ取得
	stage.LoadStage("");

	//弾初期化
	myBullet.SetPBlocks(&stage);
	myBullet.Initialize();

	//フェーズ初期化
	GameUtility::SetNowPhase(PHASE_SETPOS);

}

void Editor::Update()
{
	// DirectX 毎フレーム処理 ここから

	//入力処理ここから

#pragma region カメラ
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
#endif
#pragma endregion
#pragma region サウンド

#pragma endregion
#pragma region オブジェクト

#ifdef _DEBUG
	//リセット
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
#pragma region アップデート
	light.Update();
	camera.Update();
	timer.Update();
	//3Dサウンドで使用するリスナーの位置更新
	Sound::Set3DListenerPosAndVec(camera);

	//弾更新
	myBullet.Update();
	//ステージ更新
	stage.Update();

#pragma endregion
}

void Editor::Draw()
{
	//背景スプライト描画ここから
	Sprite::BeginDraw();
	//spr->Draw();

	//背景描画ここまで
	DX12Util::ClearDepthBuffer();

	//オブジェクト描画ここから

	//弾描画
	myBullet.Draw();

	//ステージ描画
	stage.Draw();
	

	//オブジェクト描画ここまで

	//前景スプライト描画ここから
	DebugText::DrawAll();
	ImguiHelper::Draw();

	//前景スプライト描画ここまで

}