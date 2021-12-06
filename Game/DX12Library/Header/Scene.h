#pragma once
#include <string>

class Scene
{
	using string = std::string;
protected:
	string sceneName;

public:
	virtual ~Scene() {}
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	void SetSceneName(string name) { sceneName = name; }
	string GetSceneName() { return sceneName; }
};

