#include "Title.h"
#include "DX12Util.h"
#include "GameUtility.h"
#include "Input.h"
#include "SceneManager.h"
#include "Easing.h"
#include "FPSManager.h"
#include "GameSound.h"
#include "Encorder.h"

#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "BreakFloor.h"

using namespace DX12Library;

const float Title::EFFECT_ACCEL = 0.005f;

Title::Title()
{
	//モデル
	modelBG.CreateFromOBJ(MODEL_DIR + "Sky/Sky.obj");
	//スプライト
	sprTextTitle.Initialize();
	sprTextTitle.SetTexture(TEX_DIR_TITLE + "Text_Title.png");
	sprTextClick.Initialize();
	sprTextClick.SetTexture(TEX_DIR_TITLE + "Text_Click.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(TEX_DIR_UTIL + "Black1280x720.png");
	sprWhite.Initialize();
	sprWhite.SetTexture(TEX_DIR_UTIL + "White1280x720.png");
	sprAttention.Initialize();
	sprAttention.SetTexture(TEX_DIR_TITLE + "Attention.png");
	//パーティクル
	for (int i = 0; i < _countof(particleSquare); i++) {
		particleSquare[i].LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Square.png");
		particleSquare[i].Initialize();
		particleSquare[i].SetColor(GameUtility::COLOR_VALUE[i]);
		particleSquare[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}
	for (int i = 0; i < _countof(particleTriangle); i++) {
		particleTriangle[i].LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Triangle.png");
		particleTriangle[i].Initialize();
		particleTriangle[i].SetColor(GameUtility::COLOR_VALUE[i]);
		particleTriangle[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}
	particleBreak.LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Break.png");
	particleBreak.Initialize();
	particleBreak.SetColor({ 0.71f,0.47f, 0.2f, 1 });
	particleBreak.SetBlendMode(PARTICLE_BLENDMODE_ADD);
}

Title::~Title()
{
	effectObjects.clear();
}

void Title::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(0, 0, 0);
	//カメラをセット
	DX12Util::SetCameraAll(&camera);

	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(50.0f);
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
	addEffectTimer.SetTimer(0, 1);
}

void Title::Update()
{
	UpdateTimer();
	UpdateInput();
	UpdateBG();
	UpdateEffectObjects();
	UpdateTextTex();
	UpdateAttention();
	UpdateFG();
	UpdateSound();
	//サウンド更新
	GameSound::Update();
}

void Title::Draw()
{
	DrawBG();
	DrawEffectObjects();
	DrawTextTex();
	DrawFG();
	DrawAttention();
}

void Title::UpdateInput()
{
	if (Mouse::IsMouseButtonRelease(LEFT)) {
		//左クリックでタイトルへ
		if (firstEffectTimer.GetNowTime() < 4500) {
			//黒くなり始める時間
			firstEffectTimer.SetNowTime(4500);
		}
		//左クリックでステージセレクトへ
		else if (firstEffectTimer.GetIsEnd() && sceneChangeTimer.GetIsStart() == false) {
			//シーンチェンジタイマー開始
			sceneChangeTimer.Start();
			//効果音鳴らす
			GameSound::Play(L"GameStart");
		}
	}

	//シーンチェンジタイマー終了で次のシーンに移る
	if (sceneChangeTimer.GetIsEnd()) {
		//オブジェクト削除
		effectObjects.clear();
		//シーンチェンジ
		SceneManager::ChangeScene("StageSelect");
	}
}

void Title::UpdateTimer()
{
	//開幕演出終わったらエフェクトタイマー開始
	//(以後はaddEffectTimer終了後に新しくランダムに開始される)
	//エフェクト追加
	if (firstEffectTimer.GetIsEnd() && addEffectTimer.GetIsStart() == false) {
		AddEffectObject();
		//タイマーセット
		SetAndStartAddEffectTimer();
	}
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
	//イヤホンつけたほうがいいよ～の表示の透明度更新
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
	//天球回転
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

void Title::UpdateEffectObjects()
{
	//エフェクトオブジェクト更新
	for (int i = 0; i < effectObjects.size(); i++) {
		//重力っぽいの適用
		effectObjects[i]->velocity.y -= EFFECT_ACCEL;
		//位置更新
		effectObjects[i]->position += effectObjects[i]->velocity * effectObjects[i]->moveSpeed;
		effectObjects[i]->object.SetPosition(effectObjects[i]->position);
		//回転更新
		Vector3 setRot = effectObjects[i]->object.GetRotation() + effectObjects[i]->addRotVelocity;
		effectObjects[i]->object.SetRotation(setRot);
		//オブジェクト更新
		effectObjects[i]->object.Update();
	}

	//オブジェクト削除
	auto itr = effectObjects.begin();
	while (itr != effectObjects.end())
	{
		//破壊タイマーが終了していたら
		if ((*itr)->breakTimer.GetIsEnd())
		{
			//パーティクル生成
			AddParticle(*(*itr));
			//エフェクトオブジェクト削除
			itr = effectObjects.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	//パーティクル更新
	for (int i = 0; i < _countof(particleSquare); i++) {
		particleSquare[i].Update();
	}
	for (int i = 0; i < _countof(particleTriangle); i++) {
		particleTriangle[i].Update();
	}
	particleBreak.Update();

}

void Title::UpdateSound()
{
	//開幕エフェクト終わったらBGM再生
	if (firstEffectTimer.GetIsEnd() && GameSound::IsPlaying(L"Title") == false) {
		GameSound::Play(L"Title");
	}
}

void Title::DrawTextTex()
{
	sprTextTitle.DrawFG();
	sprTextClick.DrawFG();
}

void Title::DrawAttention()
{
	//イヤホンの注意描画
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

void Title::DrawEffectObjects()
{
	//エフェクトオブジェクト描画
	for (int i = 0; i < effectObjects.size(); i++) {
		effectObjects[i]->object.Draw();
	}
	//パーティクル描画
	for (int i = 0; i < _countof(particleSquare); i++) {
		particleSquare[i].Draw();
	}
	for (int i = 0; i < _countof(particleTriangle); i++) {
		particleTriangle[i].Draw();
	}
	particleBreak.Draw();
}

void Title::AddEffectObject()
{
	//画面外からオブジェクトが飛んできてパッとはじける演出

	EffectObject* newEffectObject = new EffectObject;
	//オブジェクトを3種類に分類
	newEffectObject->type = rand() % 3;

	//オブジェクト初期化とモデルセット
	newEffectObject->object.Initialize();

	//それぞれモデルセット
	if (newEffectObject->type == PARTICLE_TYPE_SQUARE) {
		newEffectObject->object.SetObjModel(SquareBlock::GetModel(1));	//ひび割れた四角いブロックのモデル
	}
	else if (newEffectObject->type == PARTICLE_TYPE_TRIANGLE) {
		newEffectObject->object.SetObjModel(TriangleBlock::GetModel(1));	//ひび割れた三角のブロックのモデル
	}
	else if (newEffectObject->type == PARTICLE_TYPE_BREAK) {
		newEffectObject->object.SetObjModel(BreakFloor::GetModel());	//壊れる床ブロックのモデル
	}

	//球面上の位置、移動量、スピードセット
	int range = 100;
	newEffectObject->position = GetRandomOnUnitSpherePos() * range;
	//ここでオブジェクトにも座標を適用しておく
	newEffectObject->object.SetPosition(newEffectObject->position);
	//中心(のちょっと上)に近い方向へ向かう
	Vector3 center = GetRandomOnUnitSpherePos() * 30 + Vector3(0, 20, 0);
	newEffectObject->velocity = (center - newEffectObject->position).Normalize();
	newEffectObject->moveSpeed = 1.5f;
	//色セット
	newEffectObject->colorType = rand() % 5;
	//TypeがBreakでなければオブジェクトにも反映
	if (newEffectObject->type != PARTICLE_TYPE_BREAK)
	newEffectObject->object.SetColor(GameUtility::COLOR_VALUE[newEffectObject->colorType]);
	//回転を設定
	float speed = GetRandF(1, 10);//回転スピードは一定
	newEffectObject->addRotVelocity = GetRandomOnUnitSpherePos() * speed;
	//影は描画無し
	newEffectObject->object.SetDrawShadowToMyself(false);
	//破壊までのタイマーセット
	newEffectObject->breakTimer.SetTimer(0, rand() % 2000);
	newEffectObject->breakTimer.Start();

	//追加
	effectObjects.emplace_back(newEffectObject);
}

void Title::AddParticle(const EffectObject& effectObject)
{
	for (int i = 0; i < 20; i++) {
		//上方向にランダムで飛ばす
		float x = (float)((rand() % 200 - 100) * 0.01f);
		float y = (float)((rand() % 100) * 0.01f);
		float z = (float)((rand() % 200 - 100) * 0.01f);
		Vector3 vel = Vector3(x, y, z).Normalize() * 0.75f * FPSManager::GetMulAdjust60FPS();
		Vector3 acc = Vector3(0, -0.01f, 0) * FPSManager::GetMulAdjust60FPS();;
		float startScale = 6.0f;
		float endScale = 0;

		//タイプで分ける
		if (effectObject.type == PARTICLE_TYPE_SQUARE) {
			particleSquare[effectObject.colorType].Add(1000, effectObject.position, vel, acc, startScale, endScale);
		}
		else if (effectObject.type == PARTICLE_TYPE_TRIANGLE) {
			particleTriangle[effectObject.colorType].Add(1000, effectObject.position, vel, acc, startScale, endScale);
		}
		else if (effectObject.type == PARTICLE_TYPE_BREAK) {
			particleBreak.Add(1000, effectObject.position, vel, acc, startScale, endScale);
		}
	}
}

void Title::SetAndStartAddEffectTimer()
{
	//ランダムにタイマーセット
	int end = rand() % 800;

	addEffectTimer.SetTimer(0, end);
	addEffectTimer.Start();
}

float Title::GetRandF(int min, int max)
{
	int sub = (max - min);
	float randf = (float)(rand() % sub);
	float result = randf - ((max - min) / 2);
	return result;
}

Vector3 Title::GetRandomOnUnitSpherePos()
{
	//単位球上のランダムな位置計算
	//参考サイト:https://ushiostarfish.hatenablog.com/entry/2018/07/13/005942

	float x1, x2, s;
	do {
		x1 = GetRandF(-100, 100) / 100;
		x2 = GetRandF(-100, 100) / 100;
		s = x1 * x1 + x2 * x2;
	} while (s >= 1.0);

	float twoSqrtOneMinusS = 2.0 * sqrt(max(1.0 - s, 0.0));

	return Vector3(x1 * twoSqrtOneMinusS, x2 * twoSqrtOneMinusS, 1.0 - 2.0 * s);
}
