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
	//ステージロード
	for (int i = 0; i < STAGE_COUNT; i++) {
		stages.emplace_back(new Stage());
		stages[i]->LoadStage(STAGE_DIR + "stage_" + std::to_string(i) + ".spb");
		stages[i]->Initialize();
		stages[i]->SetMasterPosition(Vector3(0, 0, i * 500));
		//ステージの大きさによってカメラの位置を変える
		cameraPosList.emplace_back(Vector3(0, stages[i]->GetStageSize().y * 10, i * 500 - stages[i]->GetStageSize().y * 3 - 5));

		//ステージセレクトでは影描画無しにする
		stages[i]->SetDrawShadow(false);
	}
	//UI画像読み込み
	buttonUp.LoadTexture(TEX_DIR_STAGESELECT + L"UI_Arrow_Up.png");
	buttonDown.LoadTexture(TEX_DIR_STAGESELECT + L"UI_Arrow_Down.png");
	buttonStart.LoadTexture(TEX_DIR_STAGESELECT + L"UI_Start.png");

	//スプライト読み込み
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
	//解放
	for (int i = 0; i < stages.size(); i++) {
		delete stages[i];
		stages[i] = nullptr;
	}
	cameraPosList.clear();
}

void StageSelect::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(75, 0, 0);
	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);
	Particle3D::SetCamera(&camera);

	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(80.0f);
	//ライトをセット
	Object3D::SetLight(&light);

	//フェーズセット
	GameUtility::SetNowPhase(PHASE_STAGESELECT_SELECT);

	//UIボタン初期化
	float adjust = 10;
	buttonUp.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - buttonUp.GetTexSize().y - adjust * 2 });
	buttonDown.Initialize({ 0 + adjust, DX12Util::GetWindowHeight() - buttonDown.GetTexSize().y - adjust });
	buttonStart.Initialize({ DX12Util::GetWindowWidth() - buttonStart.GetTexSize().x - adjust, DX12Util::GetWindowHeight() - buttonStart.GetTexSize().y - adjust });

	//スプライト初期化
	sprBlack.SetColor({ 1,1,1,1 });
	sprTextStage.SetPosition({ 0, 50 });

	//タイマーセット
	firstEffectTimer.SetTimer(0, 250);
	firstEffectTimer.Start();
	startGameTimer.SetTimer(0, 2000);
	changeSelectPosTimer.SetTimer(0, 1000);
	changeSelectPosTimer.SetNowTime(changeSelectPosTimer.GetEndTime());
	roopEffectTimer.SetTimer(0, 4000, true);
	roopEffectTimer.Start();
	arrowTimer.SetTimer(0, 1500, true);
	arrowTimer.Start();

	//ステージカラー初期化
	GameUtility::SetStageColor(STAGE_COLOR_NONE);

	//BGM鳴ってなかったら再生
	if (GameSound::IsPlaying(L"Title") == false) {
		GameSound::Play(L"Title");
	}
}

void StageSelect::Update()
{
	light.Update();
	UpdateCamera();
	UpdateTimer();

	//ステージ決定前処理
	if (startGameTimer.GetIsStart() == false) {
		UpdateNowSelect();
	}
	//ステージ決定後処理
	UpdateAfterDecided();

	//ステージ更新
	UpdateStage();

	//ステージ番号テクスチャ更新
	UpdateStageNumTex();

	//ボタン更新
	UpdateButton();

	//サウンド更新
	GameSound::Update();
}

void StageSelect::Draw()
{
	//ステージ描画
	DrawStage();
	//UI描画
	DrawUI();
	//白テクスチャエフェクト描画
	DrawFG();
}

void StageSelect::UpdateCamera()
{
	//タイマー更新
	changeSelectPosTimer.Update();

	if (startGameTimer.GetIsStart() == false) {
		float add = 5, m = 4.5;
		Vector3 setPos = {};
		float lightTargetZ = 0, lightDistance = 0, mul = 0, farZ = 0;
		//パラメータセット
		if (changeSelectPosTimer.GetIsStart()) {
			if (isMoveUp) {
				//カメラ位置
				setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex - 1].y, cameraPosList[nowSelectStageIndex].y, changeSelectPosTimer);
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex - 1].z, cameraPosList[nowSelectStageIndex].z, changeSelectPosTimer);
				//ライト位置
				lightTargetZ = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex - 1) * 500, nowSelectStageIndex * 500, changeSelectPosTimer);
				lightDistance = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex - 1]->GetStageSize().y * 2.5,
					stages[nowSelectStageIndex]->GetStageSize().y * 2.5, changeSelectPosTimer);
				//ライトに関する行列更新
				mul = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex - 1]->GetStageSize().y * 0.012,
					stages[nowSelectStageIndex]->GetStageSize().y * 0.012, changeSelectPosTimer);
				farZ = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex - 1]->GetStageSize().y * m + add,
					stages[nowSelectStageIndex]->GetStageSize().y * m + add, changeSelectPosTimer);
			}
			else {
				//カメラ位置
				setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex + 1].y, cameraPosList[nowSelectStageIndex].y, changeSelectPosTimer);
				setPos.z = Easing::GetEaseValue(EASE_OUTQUINT, cameraPosList[nowSelectStageIndex + 1].z, cameraPosList[nowSelectStageIndex].z, changeSelectPosTimer);
				//ライト位置
				lightTargetZ = Easing::GetEaseValue(EASE_OUTQUINT, (nowSelectStageIndex + 1) * 500, nowSelectStageIndex * 500, changeSelectPosTimer);
				lightDistance = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex + 1]->GetStageSize().y * 2.5, 
					stages[nowSelectStageIndex]->GetStageSize().y * 2.5, changeSelectPosTimer);
				//ライトに関する行列更新
				mul = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex + 1]->GetStageSize().y * 0.012,
					stages[nowSelectStageIndex]->GetStageSize().y * 0.012, changeSelectPosTimer);
				farZ = Easing::GetEaseValue(
					EASE_OUTQUINT, stages[nowSelectStageIndex + 1]->GetStageSize().y * m + add,
					stages[nowSelectStageIndex]->GetStageSize().y * m + add, changeSelectPosTimer);
			}
		}
		else {
			//カメラ位置
			setPos = cameraPosList[nowSelectStageIndex];
			camera.SetPositionAndDistance(setPos, 15.0f);
			//ライト位置
			lightTargetZ = nowSelectStageIndex * 500;
			lightDistance = stages[nowSelectStageIndex]->GetStageSize().y * 2.5;
			//ライトに関する行列更新
			mul = stages[nowSelectStageIndex]->GetStageSize().y * 0.012;
			farZ = stages[nowSelectStageIndex]->GetStageSize().y * m + add;
		}
		//反映
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
	//移動中は矢印を消す(タイマーを最終値に)
	if (changeSelectPosTimer.GetIsStart()) {
		arrowTimer.SetNowTime(arrowTimer.GetEndTime());
	}
}

void StageSelect::UpdateNowSelect()
{
	//カーソルを上に
	if (buttonUp.IsReleaseButton()) {
		buttonUp.StartPushedEffect();
		nowSelectStageIndex++;
		if (nowSelectStageIndex >= STAGE_COUNT) { 
			nowSelectStageIndex = STAGE_COUNT - 1;
		}
		else {
			//タイマースタート
			changeSelectPosTimer.SetTimer(0, 1000);
			changeSelectPosTimer.Start();
			//上向き移動
			isMoveUp = true;
		}
	}

	//カーソルを下に
	if (buttonDown.IsReleaseButton()) {
		buttonDown.StartPushedEffect();
		nowSelectStageIndex--;
		if (nowSelectStageIndex < 0) {
			nowSelectStageIndex = 0; 
		}
		else {
			//タイマースタート
			changeSelectPosTimer.SetTimer(0, 1000);
			changeSelectPosTimer.Start();
			//下向き移動
			isMoveUp = false;
		}
	}

	//選択されたステージでゲームスタート
	if (buttonStart.IsReleaseButton()) {
		buttonStart.StartPushedEffect();
		startGameTimer.Start();
		//BGMストップ(1秒かけてフェードアウト)
		GameSound::Stop(L"Title", 1000);

		//効果音再生
		GameSound::Play(L"StageDecide");
	}

	//矢印
	
	//位置
	//上
	Vector2 setPos = { (float)DX12Util::GetWindowWidth() / 2 - sprArrowUp.GetTexSize().x / 2, 0 };//横は中心
	float start = 80, end = 20;
	setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, start, end, arrowTimer);
	sprArrowUp.SetPosition(setPos);
	//下
	float down = (float)DX12Util::GetWindowHeight() - sprArrowUp.GetTexSize().y;
	setPos.y = Easing::GetEaseValue(EASE_OUTQUINT, down - start, down - end, arrowTimer);
	sprArrowDown.SetPosition(setPos);

	//透明度
	Vector4 color = { 1,1,1,1 };
	color.w = Easing::GetEaseValue(EASE_INQUINT, 1, 0, arrowTimer);
	sprArrowUp.SetColor(color);
	sprArrowDown.SetColor(color);
}

void StageSelect::UpdateStage()
{
	//白い丸の透明度
	float digrees = (float)roopEffectTimer.GetNowTime() * 180 / roopEffectTimer.GetEndTime();
	float alpha = sin(digrees * PI / 180) * 0.5f + 0.5f;
	sprStageBG.SetColor({ 1,1,1,alpha });

	//ステージ更新
	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Update();
	}
}

void StageSelect::UpdateAfterDecided()
{
	//エフェクト終了後にゲームプレイシーンへ
	if (startGameTimer.GetIsEnd())
	{
		//ステージセット
		GameUtility::SetNowStagePath(STAGE_DIR + "stage_" + std::to_string(nowSelectStageIndex) + ".spb");
		//シーンを移す
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

	//文字チェック
	//桁数オーバーは99埋め
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
			sprStageNum[i].SetColor({ 1,1,1,0 });
		}
		//数字
		else {
			sprStageNum[i].SetColor({ 1,1,1,1 });
			sprStageNum[i].SetDrawRectangle(stoi(draw) * numWidth, 0, numWidth, numHeight);
		}

		sprStageNum[i].SetPosition(masterPos + Vector2(padding * i, addPosY));
	}

}

void StageSelect::UpdateButton()
{
	//ボタン有効設定
	//startGameTimer開始前は有効
	buttonUp.SetIsEnable(startGameTimer.GetNowTime() <= 0);
	buttonDown.SetIsEnable(startGameTimer.GetNowTime() <= 0);
	buttonStart.SetIsEnable(startGameTimer.GetNowTime() <= 0);
}

void StageSelect::DrawStage()
{
	//背景
	sprBackground.DrawBG();
	//白い円
	sprStageBG.DrawBG();
	//ステージ本体
	for (int i = 0; i < stages.size(); i++) {
		stages[i]->Draw();
	}
	//矢印
	//ゲーム開始タイマー開始前なら描画
	if (startGameTimer.GetIsStart() == false) {
		//上
		if (nowSelectStageIndex < stages.size() - 1) {
			sprArrowUp.DrawFG();
		}
		//下
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
	//黒
	float alpha = 1.0f - ((float)firstEffectTimer.GetNowTime() / firstEffectTimer.GetEndTime());
	sprBlack.SetColor({ 1,1,1,alpha });
	sprBlack.DrawFG();

	//白
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