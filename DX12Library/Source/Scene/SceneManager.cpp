#include <assert.h>
#include "Input.h"
#include "SceneManager.h"
#include "ImguiHelper.h"
#include "PostEffect.h"
#include "RenderText.h"
#include "DebugText.h"
#include "InstancingObject.h"

//静的メンバ変数の実体
std::string SceneManager::nowScene;
std::string SceneManager::nextScene = "";
std::unordered_map<std::string, Scene*> SceneManager::scenes;

void SceneManager::AddScene(Scene* scene, const string& sceneName)
{
	scene->SetSceneName(sceneName);
	scenes.emplace(sceneName, scene);
}

void SceneManager::ChangeScene(const string& sceneName)
{
	//シーンが見つかったら次のフレームからのシーンセット
	if (scenes.find(sceneName) != scenes.end()){
		nextScene = sceneName;
	}
}

void SceneManager::DeleteScene()
{
	for (auto itr = scenes.begin(); itr != scenes.end(); itr++)
	{
		delete itr->second;
	}

	scenes.clear();
}

void SceneManager::Initialize()
{
	scenes[nowScene]->Initialize();
}

void SceneManager::Update()
{
	//フレームの頭でシーン切り替え
	if (nowScene != nextScene) {
		nowScene = nextScene;
		Initialize();
	}

	scenes[nowScene]->Update();
}

void SceneManager::Draw()
{
	//リストに追加
	scenes[nowScene]->Draw();

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