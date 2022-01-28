#include "Title.h"
#include "DX12Util.h"
#include "GameUtility.h"
#include "Input.h"
#include "SceneManager.h"
#include "Easing.h"
#include "FPSManager.h"

Title::Title()
{
	modelBG.CreateFromOBJ(modelDir + "Sky/Sky.obj");
	sprTextTitle.Initialize();
	sprTextTitle.SetTexture(L"Resources/Title/Text_Title.png");
	sprTextClick.Initialize();
	sprTextClick.SetTexture(L"Resources/Title/Text_Click.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(L"Resources/Black1280x720.png");
	sprWhite.Initialize();
	sprWhite.SetTexture(L"Resources/White1280x720.png");
	sprAttention.Initialize();
	sprAttention.SetTexture(L"Resources/Attention.png");
}

Title::~Title()
{
}

void Title::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(0, 0, 0);
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

	//前景(黒)は最初透明に
	sprBlack.SetColor({ 1,1,1,0 });

	//テキスト
	sprTextTitle.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprTextTitle.GetTexSize().x * 0.5f, 150 });
	sprTextClick.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprTextClick.GetTexSize().x * 0.5f, 450 });

	//背景オブジェクト初期化
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	//タイマーセット
	firstEffectTimer.SetTimer(0, 6000);
	firstEffectTimer.Start();
	sceneChangeTimer.SetTimer(0, 2000);
	clickAlphaTimer.SetTimer(0, 3000, true);
	clickAlphaTimer.Start();
}

void Title::Update()
{
	UpdateTimer();
	UpdateInput();
	UpdateBG();
	UpdateTextTex();
	UpdateAttention();
	UpdateFG();
}

void Title::Draw()
{
	DrawBG();
	DrawTextTex();
	DrawFG();
	DrawAttention();
}

void Title::UpdateInput()
{
	//左クリックでステージセレクトへ
	if (Mouse::IsMouseButtonRelease(LEFT)) {
		if (firstEffectTimer.GetNowTime() < 5000) {
			firstEffectTimer.SetNowTime(5000);
		}
		else if (firstEffectTimer.GetIsEnd()) {
			sceneChangeTimer.Start();
		}
	}

	//シーンチェンジタイマー終了で次のシーンに移る
	if (sceneChangeTimer.GetIsEnd()) {
		SceneManager::ChangeScene("StageSelect");
	}
}

void Title::UpdateTimer()
{
	firstEffectTimer.Update();
	sceneChangeTimer.Update();
	clickAlphaTimer.Update();
}

void Title::UpdateTextTex()
{
	//「Click」のテクスチャ処理
	if (sceneChangeTimer.GetIsStart()) {
		//点滅
		if (sceneChangeTimer.GetNowTime() < 500) {
			int alpha = sceneChangeTimer.GetNowTime() * 0.02;
			alpha = alpha % 2;
			sprTextClick.SetColor({ 1,1,1,(float)alpha });
		}
		//通常描画
		else {
			sprTextClick.SetColor({ 1,1,1,1 });
		}
	}
	//ゆっくり点滅
	else {
		float digrees = (float)clickAlphaTimer.GetNowTime() * 180 / clickAlphaTimer.GetEndTime();
		float alpha = sin(digrees * PI / 180);
		sprTextClick.SetColor({ 1,1,1,alpha });
	}
}

void Title::UpdateAttention()
{
	if (firstEffectTimer.GetNowTime() < 500) {
		float alpha = (((float)firstEffectTimer.GetNowTime()) / 500);
		sprAttention.SetColor({ 1,1,1,alpha });
		sprWhite.SetColor({ 1,1,1,1 });
	}
	else if (firstEffectTimer.GetNowTime() >= 4500) {
		float alpha = 1.0f - (((float)firstEffectTimer.GetNowTime() - 4500) / 500);
		sprAttention.SetColor({ 1,1,1,alpha });
		sprWhite.SetColor({ 1,1,1,alpha });
	}
	else {
		sprAttention.SetColor({ 1,1,1,1 });
		sprWhite.SetColor({ 1,1,1,1 });
	}
}

void Title::UpdateBG()
{
	float rotY = 0.1f * FPSManager::GetMulAdjust60FPS();
	objBG.AddRotation(0, rotY, 0);
	objBG.Update();
}

void Title::UpdateFG()
{
	//フェードアウト
	if (sceneChangeTimer.GetNowTime() >= 1000) {
		float alpha = ((float)sceneChangeTimer.GetNowTime() - 1000.0f) / 500;
		sprBlack.SetColor({1,1,1,alpha});
	}
	else {
		if (firstEffectTimer.GetNowTime() >= 5500) {
			float alpha = 1.0f - (((float)firstEffectTimer.GetNowTime() - 5500) / (firstEffectTimer.GetEndTime() - 5500));
			sprBlack.SetColor({ 1,1,1,alpha });
		}
		else if (firstEffectTimer.GetNowTime() >= 4500) {
			sprBlack.SetColor({ 1,1,1,1 });
		}
	}
}

void Title::DrawTextTex()
{
	sprTextTitle.DrawFG();
	sprTextClick.DrawFG();
}

void Title::DrawAttention()
{
	if (firstEffectTimer.GetNowTime() < 5000) {
		sprAttention.DrawFG();
	}
}

void Title::DrawBG()
{
	if (firstEffectTimer.GetNowTime() >= 5000) {
		objBG.Draw();
	}
	else {
		sprWhite.DrawBG();
	}
}

void Title::DrawFG()
{
	sprBlack.DrawFG();
}
