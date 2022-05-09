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
	//フェーズセット
	GameUtility::SetNowPhase(PHASE_GAME_FIRSTEFFECT);

	//ポーズ解除
	GameUtility::SetIsPause(false);

	//カメラ初期化
	camera.Initialize();
	camera.SetPFirstEffectTimer(&firstEffectTimer);
	camera.SetPClearEffectTimer(&clearEffectTimer);

	//カメラをセット
	DX12Util::SetCameraAll(&camera);

	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	//ライトをセット
	Object3D::SetLight(&light);

	//ステージ取得
	stage.Initialize();
	stage.SetPFirstEffectTimer(&firstEffectTimer);
	stage.SetPClearEffectTimer(&clearEffectTimer);
	stage.LoadStage(GameUtility::GetNowStagePath());

	//ステージサイズからカメラ位置セット
	float bounceY = camera.SetPosFromStageSize(stage.GetStageSize());

	//ステージサイズからライト位置と行列セット
	//幅が長い方を基準にする
	float length = stage.GetStageSize().x < stage.GetStageSize().y ?
		(float)stage.GetStageSize().y : (float)stage.GetStageSize().x;

	//行列をセット
	light.CalcLightPos(length * 2.5);
	float mul = length * 0.012f;
	float farZ = length * 4.5 + 5;
	Object3D::SetMatrixOrthographicLH(1280 * mul, 720 * mul, 1.0f, farZ);

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

	//UIにあるブロック
	objUISquareBlock.Initialize();
	//一番ボロボロな見た目のモデルを取得
	objUISquareBlock.SetObjModel(SquareBlock::GetModel(1));
	objUISquareBlock.SetScale(0.075, 0.075, 0.075);
	//影描画無し
	objUISquareBlock.SetDrawShadowToOther(false);
	objUISquareBlock.SetDrawShadowToMyself(false);

	//ポップアップ表示フラグ
	isDispPopup = false;

	//ポップアップ初期化
	sprPopUp.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprPopUp.GetTexSize().x * 0.5f, DX12Util::GetWindowHeight() * 0.5f - sprPopUp.GetTexSize().y * 0.5f });
	
	//初期状態は透明
	sprPopUp.SetColor({ 1,1,1,0 });
	sprWhite.SetColor({ 1,1,1,1 });
	sprBlack.SetColor({ 1,1,1,0});
	sprTextClearTime.SetPosition({ 175,450 });
	sprUIRemainingBlock.SetPosition({ -sprUIRemainingBlock.GetTexSize().x, 95.0f });

	//「Clear」文字の初期位置(画面両端)
	float clearTextPosY = 150;
	sprTextClear[0].SetPosition({ -sprTextClear[0].GetTexSize().x, clearTextPosY });
	sprTextClear[1].SetPosition({ (float)DX12Util::GetWindowWidth(), clearTextPosY });

	//タイマー初期化
	firstEffectTimer.SetTimer(0, 4500);
	firstEffectTimer.Start();
	dispPopUpTimer.SetTimer(0, 500);
	sceneChangeTimer.SetTimer(0, 1000);
	clearEffectTimer.SetTimer(0, 10000);
	scoreTimer.SetTimer(0, INT_MAX);
	addParticleTimer.SetTimer(0, 100);

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

	//パーティクル初期化
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Initialize();
		particle[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}

	addedParticleClearEffect = false;

	//ステージカラー初期化
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//開幕エフェクト終了フラグ
	isEndFirstEffectOnce = false;

	//チュートリアル
	tutorial.Initialize(true);

	//BGM鳴ってなかったら再生
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
	//3Dサウンドで使用するリスナーの位置更新
	Sound::Set3DListenerPosAndVec(camera);

	//サウンド更新
	GameSound::Update();

	//UpdateImgui();
	//チュートリアル
	tutorial.Update();
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

	//ステージ描画
	stage.Draw();

	//弾描画
	myBullet.Draw();

	//UI描画
	DrawUI();

	//黒テクスチャ描画
	DrawBlackEffect();

	//チュートリアル描画
	tutorial.Draw();

	//クリアエフェクト描画
	DrawClearEffect();

	//ポップアップ描画
	DrawStageBackPopUp();

	//白テクスチャ描画
	DrawWhiteEffect();

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
		//最初の1回だけスコアタイマー開始
		if (isEndFirstEffectOnce == false) {
			scoreTimer.Reset();
			scoreTimer.Start();
		}
		isEndFirstEffectOnce = true;
	}
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
	//ToDo:マジックナンバー多いから何とかする

	//移動
	//開幕エフェクトの位置移動(1回だけ)
	if (isEndFirstEffectOnce == false) {
		//ブロック
		float x = Easing::GetEaseValue(EASE_OUTQUINT, -8.2f, -5.2f, firstEffectTimer, 3500, 4000);
		Vector3 addPos = { x,0,2 };
		objUISquareBlock.SetPosition(camera.GetPosition() + camera.GetCameraDir() * 5 + addPos);
		//その下のUI
		x = Easing::GetEaseValue(EASE_OUTQUINT, -sprUIRemainingBlock.GetTexSize().x, 8, firstEffectTimer, 3500, 4000);
		sprUIRemainingBlock.SetPosition({ x, sprUIRemainingBlock.GetPosition().y });
		//残りブロック数
		x = Easing::GetEaseValue(EASE_OUTQUINT, -200, 128, firstEffectTimer, 3500, 4000);
		SetRemainingBlockCountTexPos(sprRemainingBlockCount, _countof(sprRemainingBlockCount), { x, 116 }, 40);
		//UIボタン
		x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonReset.GetTexSize().x, 10, firstEffectTimer, 3750, 4250);
		buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
		x = Easing::GetEaseValue(EASE_OUTQUINT, -buttonBack.GetTexSize().x, 10, firstEffectTimer, 4000, 4500);
		buttonBack.SetPosition({ x, buttonBack.GetPosition().y });
	}
	//クリアエフェクトの位置移動
	else if (clearEffectTimer.GetIsStart()) {
		//ブロック
		float x = Easing::GetEaseValue(EASE_INBACK, -5.2f, -8.2f, clearEffectTimer, 500, 1000);
		Vector3 addPos = { x,0,2 };
		objUISquareBlock.SetPosition(camera.GetPosition() + camera.GetCameraDir() * 5 + addPos);
		//その下のUI
		x = Easing::GetEaseValue(EASE_INBACK, 8, -sprUIRemainingBlock.GetTexSize().x, clearEffectTimer, 500, 1000);
		sprUIRemainingBlock.SetPosition({ x, sprUIRemainingBlock.GetPosition().y });
		//残りブロック数
		x = Easing::GetEaseValue(EASE_INBACK, 128, -200, clearEffectTimer, 500, 1000);
		SetRemainingBlockCountTexPos(sprRemainingBlockCount, _countof(sprRemainingBlockCount), { x, 116 }, 40);
		//UIボタン
		x = Easing::GetEaseValue(EASE_INBACK, 10, -buttonReset.GetTexSize().x, clearEffectTimer, 750, 1250);
		buttonReset.SetPosition({ x, buttonReset.GetPosition().y });
		x = Easing::GetEaseValue(EASE_INBACK, 10, -buttonBack.GetTexSize().x, clearEffectTimer, 1000, 1500);
		buttonBack.SetPosition({ x, buttonBack.GetPosition().y });
	}

	//ブロック回転
	static float add = 0;
	add += 1.0f * FPSManager::GetMulAdjust60FPS();
	if (add >= 360) { add -= 360; }
	objUISquareBlock.SetRotation(camera.GetRotation() + Vector3(add, add, add));
	objUISquareBlock.Update();

	//残りブロック数セット
	SetRemainingBlockCountTex(sprRemainingBlockCount, _countof(sprRemainingBlockCount), stage.GetTargetBlockCount(), 48, 64);

	//ボタン入力
	if (GameUtility::GetIsPause() == false) {
		if (buttonReset.IsReleaseButton()) {
			buttonReset.StartPushedEffect();
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
			//球の転がる効果音停止
			GameSound::Stop(L"Shooting");
			//ボタンを押せるように
			buttonYes.SetIsEnable(true);
			buttonNo.SetIsEnable(true);
		}
	}


	//ポーズ中とクリア時はボタン無効化
	bool isClear = stage.GetTargetBlockCount() <= 0;
	buttonReset.SetIsEnable(!isDispPopup && !isClear);
	buttonBack.SetIsEnable(!isDispPopup && !isClear);
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
				//ボタンを押せないように
				buttonYes.SetIsEnable(false);
				buttonNo.SetIsEnable(false);
				//BGMストップ(1秒かけてフェードアウト)
				GameSound::Stop(L"GamePlay", 1000);
			}
			if (buttonNo.IsReleaseButton()) {
				buttonNo.StartPushedEffect();
				dispPopUpTimer.Reset();
				dispPopUpTimer.Start();
				isDispPopup = false;
				//ボタンを押せないように
				buttonYes.SetIsEnable(false);
				buttonNo.SetIsEnable(false);
			}
		}
		
		//ポップアップタイマー終了でポーズ解除
		if (isDispPopup == false && dispPopUpTimer.GetIsEnd()) {
			GameUtility::SetIsPause(false);
			//スコアタイマー再スタート
			scoreTimer.Start();
		}
	}
}

void GamePlay::UpdateClearEffect()
{
	//クリアエフェクト中のみ更新
	if (clearEffectTimer.GetNowTime() <= clearEffectTimer.GetStartTime()) {
		return;
	}

	const int START_MOVE_TEXT_TIME = 5000;
	const int END_MOVE_TEXT_TIME = START_MOVE_TEXT_TIME + 250;
	const int START_WHITE_EFFECT = 5750;
	const int START_SCORE_TIMER_EFFECT = 6250;
	const int START_SCORE_TIMER_EFFECT_ROLL_NUMBER = 6750;
	const int START_OK_BUTTON_EFFECT = 8500;

	//クリア文字を画面の両端から中心に
	for (int i = 0; i < _countof(sprTextClear); i++) {
		//テクスチャ移動開始終了位置
		float startPosX = 0, endPosX = DX12Util::GetWindowWidth() * 0.5f - sprTextClear[i].GetTexSize().x * 0.5f;
		if (i == 0) { startPosX = -sprTextClear[i].GetTexSize().x; }
		else { startPosX = DX12Util::GetWindowWidth(); }

		Vector2 texPos = { startPosX, 150 };
		texPos.x = Easing::GetEaseValue(EASE_LINE, startPosX, endPosX, clearEffectTimer, START_MOVE_TEXT_TIME, END_MOVE_TEXT_TIME);

		sprTextClear[i].SetPosition(texPos);
	}

	//白テクスチャと黒テクスチャ
	if (clearEffectTimer.GetNowTime() >= START_WHITE_EFFECT) {
		//黒テクスチャ透明度更新
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 0.5f, clearEffectTimer, START_WHITE_EFFECT, START_WHITE_EFFECT + 500);
		Vector4 color = sprBlack.GetColor();
		color.w = alpha;
		sprBlack.SetColor(color);
	}
	else{
		sprBlack.SetColor({ 1,1,1,0 });
	}

	//スコアタイマー
	if (clearEffectTimer.GetNowTime() >= START_SCORE_TIMER_EFFECT) {
		//「ClearTime」文字の透明度更新
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 1, clearEffectTimer, START_SCORE_TIMER_EFFECT, START_SCORE_TIMER_EFFECT_ROLL_NUMBER);
		Vector4 color = sprTextClearTime.GetColor();
		color.w = alpha;
		sprTextClearTime.SetColor(color);

		//スコアタイム数字の準備
		SetScoreTimeTex(sprTextTimeNumber, _countof(sprTextTimeNumber), START_SCORE_TIMER_EFFECT_ROLL_NUMBER);

		//「s」と「.」を抜いた文字数
		int numberCount = (GetStrScoreTime(-1).size() - 2);
		int endSound = START_SCORE_TIMER_EFFECT_ROLL_NUMBER + numberCount * 250;

		//効果音再生
		if (GameSound::IsPlaying(L"RollNumber") == false && 
			clearEffectTimer.GetNowTime() >= START_SCORE_TIMER_EFFECT_ROLL_NUMBER &&
			clearEffectTimer.GetNowTime() < endSound) {
			GameSound::Play(L"RollNumber");
		}
		//効果音停止
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

	//OKボタン
	if (clearEffectTimer.GetNowTime() >= START_OK_BUTTON_EFFECT) {
		float adjust = 10;
		float x = Easing::GetEaseValue(EASE_OUTQUINT, DX12Util::GetWindowWidth(), DX12Util::GetWindowWidth() - buttonOK.GetTexSize().x - adjust, clearEffectTimer, START_OK_BUTTON_EFFECT, START_OK_BUTTON_EFFECT + 500);
		buttonOK.SetPosition({ x, buttonOK.GetPosition().y });

		//OKボタン押された
		if (buttonOK.IsReleaseButton()) {
			buttonOK.StartPushedEffect();
			sceneChangeTimer.Start();
			buttonOK.SetIsEnable(false);
			//BGMストップ(1秒かけてフェードアウト)
			GameSound::Stop(L"GamePlay", 1000);
		}
	}

	//はじけ飛ぶパーティクル追加とクリア効果音
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

		//クリア効果音
		GameSound::Play(L"Clear");
	}

	//キラキラするパーティクル追加
	if (addParticleTimer.GetIsEnd()) {
		float x = (float)((rand() % 550 - 275));
		float y = (float)((rand() % 300 - 150));

		Vector2 pos = Vector2(DX12Util::GetWindowWidth() / 2, 260) + Vector2(x, y);

		particle[1].Add(1000, pos, {}, {}, 0, 0, 0.1f);
		addParticleTimer.Reset();
		addParticleTimer.Start();
	}


	//パーティクル更新
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Update();
	}
}

void GamePlay::UpdateChangeScene()
{
	//シーンチェンジ更新
	if (sceneChangeTimer.GetIsStart()) {
		//白テクスチャ透明度更新
		float alpha = Easing::GetEaseValue(EASE_INSINE, 0, 1, sceneChangeTimer, 0, 1000);
		//白テクスチャを黒として扱う
		Vector4 color = { 0,0,0,alpha };
		sprWhite.SetColor(color);
	}

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
	//ボタン描画
	buttonReset.Draw();
	buttonBack.Draw();
	//ブロック描画
	if (firstEffectTimer.GetNowTime() >= 3500 || isEndFirstEffectOnce) {
		objUISquareBlock.Draw();
	}
	//その下のUI描画
	sprUIRemainingBlock.DrawFG();
	//残りブロック数描画
	for (int i = 0; i < _countof(sprRemainingBlockCount); i++) {
		sprRemainingBlockCount[i].DrawFG();
	}
}

void GamePlay::DrawStageBackPopUp()
{
	//ポーズ中に描画
	if (GameUtility::GetIsPause()) {
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

	//パーティクル
	for (int i = 0; i < _countof(particle); i++) {
		particle[i].Draw();
	}
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
	firstEffectTimer.SetTimer(MYBULLET_START_FIRST_EFFECT_TIME, 4500);
	firstEffectTimer.Start();

	//クリアエフェクトタイマーリセット
	clearEffectTimer.Reset();

	//弾初期化
	myBullet.Initialize();

	//念のためカメラを定位置に
	camera.SetCameraParamAfterShoot();
	camera.Update();

	//念のため白と黒テクスチャ透明度0に
	sprWhite.SetColor({ 1,1,1,0 });
	sprBlack.SetColor({ 1,1,1,0 });

	//ステージカラー初期化
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//UI座標を定位置に
	//ブロック
	Vector3 addPos = { -5.2f,0,2 };
	objUISquareBlock.SetPosition(camera.GetPosition() + camera.GetCameraDir() * 5 + addPos);
	//その下のUI
	sprUIRemainingBlock.SetPosition({ 8, sprUIRemainingBlock.GetPosition().y });
	//残りブロック数
	SetRemainingBlockCountTexPos(sprRemainingBlockCount, _countof(sprRemainingBlockCount), { 128, 116 }, 40);
	//UIボタン
	buttonReset.SetPosition({ 10, buttonReset.GetPosition().y });
	buttonBack.SetPosition({ 10, buttonBack.GetPosition().y });
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

		//球の転がる効果音停止
		GameSound::Stop(L"Shooting");
	}
}

void GamePlay::SetRemainingBlockCountTex(Sprite* pNumberTexArray, int arraySize, int drawNum, float numWidth, float numHeight)
{
	//描画する文字列
	std::string drawStr = std::to_string(drawNum);

	//文字チェック
	//桁数オーバーは999埋め
	if (drawStr.size() > arraySize) {
		drawStr = "";
		for (int i = 0; i < arraySize; i++) {
			drawStr += "9";
		}
	}
	else {
		//穴埋め
		while (drawStr.size() < arraySize) {
			drawStr = "#" + drawStr;
		}
	}

	for (int i = 0; i < arraySize; i++) {
		//描画する1文字
		const std::string draw = drawStr.substr(i, 1);

		//描画しない
		if (draw == "#") {
			pNumberTexArray[i].SetColor({ 1,1,1,0 });
		}
		//数字
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
			pNumberTexArray[i].SetDrawRectangle(0, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
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

			pNumberTexArray[i].SetDrawRectangle(drawNum * ONE_TEX_SIZE.x, 0, ONE_TEX_SIZE.x, ONE_TEX_SIZE.y);
		}

		//描画位置より前の数字は描画しない
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

	//ketaが-1じゃないとき、文字列がketaと同じ桁数になるように#で埋める
	if (keta != -1) {
		while (str.size() < keta) {
			str = "#" + str;
		}
	}

	return str;
}
