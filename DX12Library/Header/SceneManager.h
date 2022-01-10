#pragma once
#include <string>
#include <unordered_map>
#include "Scene.h"

class SceneManager
{
	using string = std::string;
	template <typename T,typename U>
	using unordered_map = std::unordered_map<T,U>;

private:
	static string nowScene;
	static string nextScene;
	static unordered_map<string, Scene*> scenes;

public:
	static void AddScene(Scene* scene, const string& sceneName);
	static void ChangeScene(const string& sceneName);
	static void DeleteScene();

	static void Initialize();
	static void Update();
	static void Draw();
};

