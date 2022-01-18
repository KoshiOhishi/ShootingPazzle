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
	buttonOK.LoadTexture(L"Resources/UI/UI_OK.png");
	sprWhite.Initialize();
	sprWhite.SetTexture(L"Resources/Write1280x720.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(L"Resources/Black1280x720.png");
	sprPopUp.Initialize();
	sprPopUp.SetTexture(L"Resources/Game_PopUp.png");
	for (int i = 0; i < _countof(sprTextClear); i++) {
		sprTextClear[i].Initialize();
		sprTextClear[i].SetTexture(L"Resources/Text_Clear.png");
	}
	sprTextClearTime.Initialize();
	sprTextClearTime.SetTexture(L"Resources/Text_ClearTime.png");
	for (int i = 0; i < _countof(sprTextTimeNumber); i++) {
		sprTextTimeNumber[i].Initialize();
		sprTextTimeNumber[i].SetTexture(L"Resources/Text_TimeNumber.png");
	}
}

GamePlay::~GamePlay()
{
}

void GamePlay::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//フェーズセット
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//ポーズ解除
	GameUtility::SetIsPause(false);

	//カメラ初期化
	camera.Initialize();
	camera.SetPFirstEffectTimer(&firstEffectTimer);
	camera.SetPClearEffectTimer(&clearEffectTimer);

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
	stage.SetPFirstEffectTimer(&firstEffectTimer);
	stage.SetPClearEffectTimer(&clearEffectTimer);
	stage.LoadStage(GameUtility::GetNowStagePath());

	//ステージサイズからカメラ位置セット
	float bounceY = camera.SetPosFromStageSize(stage.GetStageSize());

	//弾初期化
	myBullet.SetPStage(&stage);
	myBullet.Initialize();
	myBullet.SetBounceInitPosY(bounceY);
	myBullet.SetPFirstEffectTimer(&firstEffectTimer);

	//背景オブジェクト初期化
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	//ポップアップ表示フラグ
	isDispPopup = false;

	//ポップアップ初期化
	sprPopUp.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprPopUp.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.5f - sprPopUp.GetTexSize().y * 0.5f });
	
	//初期状態は透明
	sprPopUp.SetColor({ 1,1,1,0 });
	sprWhite.SetColor({ 1,1,1,1 });
	sprBlack.SetColor({ 1,1,1,0});
	sprTextClearTime.SetPosition({ 175,450 });

	//「Clear」文字の初期位置(画面両端)
	float clearTextPosY = 150;
	sprTextClear[0].SetPosition({ -sprTextClear[0].GetTexSize().x, clearTextPosY });
	sprTextClear[1].SetPosition({ (float)DX12Util::GetWindowWidth(), clearTextPosY });

	//タイマー初期化
	firstEffectTimer.SetTimer(0, 4250);
	firstEffectTimer.Start();
	dispPopUpTimer.SetTimer(0, 500);
	sceneChangeTimer.SetTimer(0, 1000);
	clearEffectTimer.SetTimer(0, 10000);
	scoreTimer.SetTimer(0, INT_MAX);

	//UIボタン初期化
	float adjust = 10;
	buttonReset.Initialize({ -buttonReset.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - buttonReset.GetTexSize().y - adjust * 2 });
	buttonBack.Initialize({ -buttonBack.GetTexSize().x, DX12Util::GetWindowHeight() - buttonBack.GetTexSize().y - adjust });
	buttonYes.Initialize({ DX12Util::GetWindowWidth() * 0.375f - buttonYes.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.75f - buttonYes.GetTexSize().y * 0.5f});
	buttonOK.Initialize({ (float)DX12Util::GetWindowWidth(), (float)DX12Util::GetWindowHeight() - buttonOK.GetTexSize().y - adjust });
	//初期状態は透明
	buttonYes.SetColor({ 1,1,1,0 });
	buttonNo.Initialize({ DX12Util::GetWindowWidth() * 0.625f - buttonNo.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.75f - buttonNo.GetTexSize().y * 0.5f });
	//初期状態は透明
	buttonNo.SetColor({ 1,1,1,0 });


	//ステージカラー初期化
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//開幕エフェクト終了フラグ
	isEndFirstEffectOnce = false;
}

void GamePlay::Update()
{
	UpdateTimer();
	UpdateDebugCamera();
	camera.Update();
	light.Update();
	//3Dサウンドで使用するリスナーの位置更新
	Sound::Set3DListenerPosAndVec(camera);
	//UpdateImgui();
	//UI更新
	UpdateUI();
	//ポップアップ更新
	UpdateStageBackPopUp();

	//クリアしているか？
	CheckIsClear();
	//背景オブジェクト更新
	UpdateBG();
	//弾更新
	myBullet.Update();
	//ステージ更新
	stage.Update();

	//白エフェクト更新
	UpdateFirstEffect();

	//クリアエフェクト更新
	UpdateClearEffect();

	//シーンチェンジ更新
	UpdateChangeScene();
	
}

void GamePlay::Draw()
{
	//背景描画
	objBG.Draw();

	//弾描画
	myBullet.Draw();

	//ステージ描画
	stage.Draw();

	//UI描画
	DrawUI();

	//黒テクスチャ描画
	DrawBlackEffect();

	//クリアエフェクト描画
	DrawClearEffect();

	//ポップアップ描画
	DrawStageBackPopUp();

	//白テクスチャ描画
	DrawWhiteEffect();

	DebugText::Print(scoreTimer.GetNowTime(), 0, 20);

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

void GamePlay::UpdateTimer()
{
	//開幕エフェクトが終了したら球の位置決めフェーズへ
	if (firstEffectTimer.GetIsEnd() == true &&
		GameUtility::GetNowPhase() == PHASE_GAME_FIRSTEFFECT) {
		GameUtility::SetNowPhase(PHASE_GAME_SETPOS);
		isEndFirstEffectOnce = true;
		scoreTimer.Reset();
		scoreTimer.Start();
	}

	firstEffectTimer.Update();
	scoreTimer.Update();
	dispPopUpTimer.Update();
	sceneChangeTimer.Update();
	clearEffectTimer.Update();
}

void GamePlay::UpdateFirstEffect()
{
	//完全に透明でなければ更新
	if (firstEffectTimer.GetIsEnd() == true) {
		return;
	}

	//白テクスチャ透明度更新
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
	//ボタン移動
	//開幕エフェクトの位置移動(1回だけ)
	if (isEndFirstEffectOnce == false) {
		float x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonReset.GetTexSize().x, 10, firstEffectTimer, 3500, 4000);
		buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
		x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonBack.GetTexSize().x, 10, firstEffectTimer, 3750, 4250);
		buttonBack.SetPosition({ x, buttonBack.GetPosition().y });

		//エフェクト中はボタン入力させない
		return;
	}
	//クリアエフェクトの位置移動
	else if (clearEffectTimer.GetIsStart()) {
		float x = Easing::GetEaseValue(EASE_INBACK, 10, -buttonReset.GetTexSize().x, clearEffectTimer, 500, 1000);
		buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
		x = Easing::GetEaseValue(EASE_INBACK, 10, -buttonBack.GetTexSize().x, clearEffectTimer, 750, 1250);
		buttonBack.SetPosition({ x, buttonBack.GetPosition().y });

		//エフェクト中はボタン入力させない
		return;
	}

	//ボタン入力
	if (GameUtility::GetIsPause() == false) {
		buttonReset.SetIsDispOverlapMouseTex(true);
		buttonBack.SetIsDispOverlapMouseTex(true);

		if (buttonReset.IsReleaseButton()) {
			buttonReset.StartPushedEffect();
			//スコアタイマーリセット
			scoreTimer.Reset();
			Reset();
		}
		if (buttonBack.IsReleaseButton()) {
			buttonBack.StartPushedEffect();
			//ポップアップタイマースタート
			dispPopUpTimer.Reset();
			dispPopUpTimer.Start();
			isDispPopup = true;
			//ポーズ状態にする
			GameUtility::SetIsPause(true);
			//スコアタイマー一時停止
			scoreTimer.Stop();
		}
	}
	else {
		//ポーズ中はカーソルとボタンが重なってもテクスチャを変化させない
		if (dispPopUpTimer.GetIsEnd()) {
			buttonReset.SetIsDispOverlapMouseTex(false);
			buttonBack.SetIsDispOverlapMouseTex(false);
		}

	}
}

void GamePlay::UpdateStageBackPopUp()
{

	if (GameUtility::GetIsPause()) {
		//ポップアップの透明度変化
		if (dispPopUpTimer.GetIsStart()) {
			float alpha = 0;
			//ポップアップ表示オンなら0から1へ
			if (isDispPopup) {
				alpha = Easing::GetEaseValue(EASE_OUTQUINT, 0, 1, dispPopUpTimer);
			}
			//そうでないなら1→0へ
			else {
				alpha = Easing::GetEaseValue(EASE_OUTQUINT, 1, 0, dispPopUpTimer);
			}
			sprPopUp.SetColor({ 1,1,1, alpha });
			sprBlack.SetColor({ 1,1,1, alpha * 0.5f });
			buttonYes.SetColor({ 1,1,1, alpha });
			buttonNo.SetColor({ 1,1,1, alpha });
		}

		//シーンチェンジしていなければボタン受付
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
		
		//ポップアップタイマー終了でポーズ解除
		if (isDispPopup == false && dispPopUpTimer.GetIsEnd()) {
			GameUtility::SetIsPause(false);
			//スコアタイマー再スタート
			scoreTimer.Start();
		}
	}

	//シーンチェンジ更新
	if (sceneChangeTimer.GetIsStart()) {
		//白テクスチャ透明度更新
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 1, sceneChangeTimer, 0, 1000);
		//白テクスチャを黒として扱う
		Vector4 color = {0,0,0,alpha};
		sprWhite.SetColor(color);
	}
}

void GamePlay::UpdateClearEffect()
{
	//クリアエフェクト中のみ更新
	if (clearEffectTimer.GetNowTime() <= clearEffectTimer.GetStartTime()) {
		return;
	}

	const int END_MOVE_TEXT_TIME = 5250;

	//クリア文字を画面の両端から中心に
	for (int i = 0; i < _countof(sprTextClear); i++) {
		//テクスチャ移動開始終了位置
		float startPosX = 0, endPosX = DX12Util::GetWindowWidth() * 0.5f - sprTextClear[i].GetTexSize().x * 0.5f;
		if (i == 0) { startPosX = -sprTextClear[i].GetTexSize().x; }
		else { startPosX = DX12Util::GetWindowWidth(); }

		Vector2 texPos = { startPosX, 150 };
		texPos.x = Easing::GetEaseValue(EASE_LINE, startPosX, endPosX, clearEffectTimer, 5000, END_MOVE_TEXT_TIME);

		sprTextClear[i].SetPosition(texPos);
	}

	//白テクスチャと黒テクスチャ
	if (clearEffectTimer.GetNowTime() >= END_MOVE_TEXT_TIME + 500) {
		//黒テクスチャ透明度更新
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 0.5f, clearEffectTimer, END_MOVE_TEXT_TIME + 500, END_MOVE_TEXT_TIME + 1000);
		Vector4 color = sprBlack.GetColor();
		color.w = alpha;
		sprBlack.SetColor(color);
	}
	else{
		sprBlack.SetColor({ 1,1,1,0 });
	}

	//スコアタイマー
	if (clearEffectTimer.GetNowTime() >= END_MOVE_TEXT_TIME + 1000) {
		//「ClearTime」文字の透明度更新
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 1, clearEffectTimer, END_MOVE_TEXT_TIME + 1000, END_MOVE_TEXT_TIME + 1250);
		Vector4 color = sprTextClearTime.GetColor();
		color.w = alpha;
		sprTextClearTime.SetColor(color);

		//スコアタイム数字の準備
		SetScoreTimeTex(sprTextTimeNumber, _countof(sprTextTimeNumber), END_MOVE_TEXT_TIME + 1500);
	}
	else {
		sprTextClearTime.SetColor({ 1,1,1,0 });
		for (int i = 0; i < _countof(sprTextTimeNumber); i++) {
			sprTextTimeNumber[i].SetColor({ 1,1,1,0 });
		}
	}

	//OKボタン
	if (clearEffectTimer.GetNowTime() >= END_MOVE_TEXT_TIME + 2500) {
		float adjust = 10;
		float x = Easing::GetEaseValue(EASE_OUTQUINT, DX12Util::GetWindowWidth(), DX12Util::GetWindowWidth() - buttonOK.GetTexSize().x - adjust, clearEffectTimer, END_MOVE_TEXT_TIME + 2500, END_MOVE_TEXT_TIME + 3000);
		buttonOK.SetPosition({ x, buttonOK.GetPosition().y });

		if (buttonOK.IsReleaseButton()) {
			buttonOK.StartPushedEffect();
			sceneChangeTimer.Start();
		}
	}
}

void GamePlay::UpdateChangeScene()
{
	//タイマー終了でシーンチェンジ実行
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
	buttonReset.Draw();
	buttonBack.Draw();
}

void GamePlay::DrawStageBackPopUp()
{
	//エフェクト中もしくはポーズ中に描画
	if (GameUtility::GetIsPause() || dispPopUpTimer.GetIsEnd() == false) {
		sprPopUp.DrawFG();
		buttonYes.Draw();
		buttonNo.Draw();
	}
}

void GamePlay::DrawClearEffect()
{
	//クリアエフェクト中のみ描画
	if (clearEffectTimer.GetNowTime() <= clearEffectTimer.GetStartTime()) {
		return;
	}

	//半透明黒テクスチャ
	sprBlack.DrawFG();

	//「Clear」文字
	for (int i = 0; i < _countof(sprTextClear); i++) {
		sprTextClear[i].DrawFG();
	}

	//「ClearText」文字
	sprTextClearTime.DrawFG();

	//スコアタイム文字
	for (int i = 0; i < _countof(sprTextTimeNumber); i++) {
		sprTextTimeNumber[i].DrawFG();
	}

	//OKボタン
	buttonOK.Draw();
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

	//球のエフェクトのみ反映するような範囲
	firstEffectTimer.SetTimer(MYBULLET_START_FIRST_EFFECT_TIME, 4250);
	firstEffectTimer.Start();

	//クリアエフェクトタイマーリセット
	clearEffectTimer.Reset();

	//弾初期化
	myBullet.Initialize();

	//念のためカメラを定位置に
	camera.SetCameraParamAfterShoot();

	//念のため白と黒テクスチャ透明度0に
	sprWhite.SetColor({ 1,1,1,0 });
	sprBlack.SetColor({ 1,1,1,0 });

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
		if (clearEffectTimer.GetIsStart() == false) {
			clearEffectTimer.Start();
		}
		//スコアタイマー停止
		scoreTimer.Stop();
	}
}

void GamePlay::SetScoreTimeTex(Sprite* pNumberTexArray, int arraySize, int startDrawTime)
{
	//数字テクスチャ1つ分の大きさ
	const Vector2 ONE_TEX_SIZE = { 72,96 };
	//スコアタイムの左上座標
	const Vector2 NUMBER_MASTER_POS = { 575,450 };
	//桁分ずらす用
	float drawPosX = 0;
	//桁が増えるとこの数値分右にずれて描画される
	float padding = 60;
	//描画する文字列
	const std::string DRAW_STR = GetStrScoreTime(arraySize);

	//描画開始位置（sは最初から描画する）
	int start = arraySize;
	if (clearEffectTimer.GetNowTime() >= startDrawTime) {
		start = arraySize - 1 - ((clearEffectTimer.GetNowTime() - startDrawTime) * 0.004f);

		//小数点は演出を飛ばす
		if (start <= arraySize - 5) {
			start--;
		}
	}

	for (int i = 0; i < arraySize; i++) {
		//描画する1文字
		const std::string draw = DRAW_STR.substr(i,1);

		pNumberTexArray[i].SetPosition(NUMBER_MASTER_POS + Vector2{ drawPosX, 0 });
		pNumberTexArray[i].SetColor({ 1,1,1,1 });

		//描画しない部分
		if (draw == "#") {
			SetRectangleNumberTex(&pNumberTexArray[i], 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
			pNumberTexArray[i].SetColor({ 1,1,1,0 });
		}
		//小数点
		else if (draw == ".") {
			//位置調整
			padding = 20;
			pNumberTexArray[i].SetPosition(NUMBER_MASTER_POS + Vector2{ drawPosX - 20, 0 });
			pNumberTexArray[i].SetDrawRectangle(ONE_TEX_SIZE.x * 11, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
		}
		//「s」
		else if (draw == "s") {
			pNumberTexArray[i].SetDrawRectangle(ONE_TEX_SIZE.x * 10, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
		}
		//数字部分
		else {
			int drawNum = stoi(draw);
			if (i == start) {
				drawNum = rand() % 10;
			}

			SetRectangleNumberTex(&pNumberTexArray[i], drawNum, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
		}

		//描画位置より前の数字は描画しない
		if (i < start) {
			pNumberTexArray[i].SetColor({ 1,1,1,0 });
		}

		drawPosX += padding;
		padding = 60;
	}
}

void GamePlay::SetRectangleNumberTex(Sprite* pNumberTex, const unsigned int num, const float numWidth, const float numHeight)
{
	pNumberTex->SetDrawRectangle(num * numWidth, 0, numWidth, numHeight);
}

std::string GamePlay::GetStrScoreTime(int keta)
{
	std::string str = std::to_string(scoreTimer.GetNowTime());

	str = str.insert(str.size() - 3, ".");
	str = str.insert(str.size(), "s");

	//文字列がketaと同じ桁数になるように#で埋める
	while (str.size() < keta) {
		str = "#" + str;
	}

	return str;
}
