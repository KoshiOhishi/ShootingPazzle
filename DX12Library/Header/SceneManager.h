#pragma once
#include <string>
#include <vector>
#include "Scene.h"

class SceneManager
{
	using string = std::string;
	template <typename T>
	using vector = std::vector<T>;

private:
	static string nowScene;
	static vector<Scene*> scenes;
	static int sceneNum;

public:
	static void AddScene(Scene* scene, const string& sceneName);
	static void SetScene(const string& sceneName);
	static void DeleteScene();

	static void Initialize();
	static void Update();
	static void Draw();
};

