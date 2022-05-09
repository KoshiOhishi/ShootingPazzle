#pragma once
#include <string>
#include <unordered_map>
#include "Scene.h"
#include <memory>

namespace DX12Library {

	class SceneManager
	{
		using string = std::string;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

	private:
		static string nowScene;
		static string nextScene;
		static unordered_map<string, std::unique_ptr<Scene>> scenes;

	public:
		static void AddScene(Scene* scene, const string& sceneName);
		static void ChangeScene(const string& sceneName);
		static void DeleteScene();

		static void Initialize();
		static void Update();
		static void Draw();
	};

}