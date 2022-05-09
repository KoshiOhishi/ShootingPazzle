#include <assert.h>
#include "Input.h"
#include "SceneManager.h"
#include "ImguiHelper.h"
#include "PostEffect.h"
#include "DebugText.h"
#include "DrawManager.h"
#include "Timer.h"

using namespace DX12Library;

//静的メンバ変数の実体
std::string SceneManager::nowScene;
std::string SceneManager::nextScene = "";
std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::scenes;

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

	//タイマーアップデート
	Timer::UpdateAll();

	scenes[nowScene]->Update();
}

void SceneManager::Draw()
{
	//リストに追加
	scenes[nowScene]->Draw();

	//デバッグテキストのスプライト前景描画登録
	DebugText::DrawAll();

	//まず深度値テクスチャ書き込み
	DrawManager::DrawAllShadow();

	//次にポストエフェクト用テクスチャに描画
	PostEffect::PreDrawScene();
	//背景描画
	DrawManager::DrawAll(true);

	//深度バッファクリア
	DX12Util::ClearDepthBuffer(false);

	//通常描画
	DrawManager::DrawAll();

	//Imgui描画
	ImguiHelper::Draw();

	//描画を終える
	PostEffect::PostDrawScene();
}