#include <assert.h>
#include "Input.h"
#include "SceneManager.h"
#include "ImguiHelper.h"
#include "PostEffect.h"
#include "RenderText.h"
#include "DebugText.h"

//静的メンバ変数の実体
SceneManager::string SceneManager::nowScene;
SceneManager::vector<Scene*> SceneManager::scenes;
int SceneManager::sceneNum;

void SceneManager::AddScene(Scene* scene, const string& sceneName)
{
	scene->SetSceneName(sceneName);
	scenes.push_back(scene);
}

void SceneManager::SetScene(const string& sceneName)
{
	nowScene = sceneName;
	for (int i = 0; i < scenes.size(); i++)
	{
		//現在シーンのみ適用
		if (scenes[i]->GetSceneName() != nowScene) continue;

		sceneNum = i;
		Initialize();
		return;
	}
}

void SceneManager::DeleteScene()
{
	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}

	scenes.clear();
}

void SceneManager::Initialize()
{
	scenes[sceneNum]->Initialize();
}

void SceneManager::Update()
{
	scenes[sceneNum]->Update();
}

void SceneManager::Draw()
{
	//リストに追加
	scenes[sceneNum]->Draw();

	//デバッグテキストのスプライト前景描画登録
	DebugText::DrawAll();

	//まず深度値テクスチャ書き込み
	Object3D::WriteDepthTex();

	//次にポストエフェクト用テクスチャに描画
	PostEffect::PreDrawScene();
	//背景スプライト描画
	Sprite::DrawAllBG();

	//背景テキスト描画
	RenderText::DrawAllBG();

	//深度バッファクリア
	DX12Util::ClearDepthBuffer(false);

	//オブジェクト描画
	Object3D::DrawAll();

	//前景スプライト描画
	Sprite::DrawAllFG();

	//前景テキスト描画
	RenderText::DrawAllFG();

	//Imgui描画
	ImguiHelper::Draw();

	//描画を終える
	PostEffect::PostDrawScene();
}