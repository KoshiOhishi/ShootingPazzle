#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "Light.h"
#include "Camera.h"
#include "Object3D.h"
#include "TouchableObject.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Sound.h"
#include "RenderText.h"
#include "CollisionManager.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "Stage.h"

#define PI 3.14159265359f

class GamePlay :
	public Scene
{
private:
	//ライト
	Light light;
	//カメラ
	Camera camera;
	//弾
	MyBullet myBullet;
	//ステージ
	Stage stage;
	//ステージパス
	std::string stagePass = ".. / StageEditor/StageData/test.spb";

	//背景
	Object3D objBG;
	ObjModel modelBG;

public:
	GamePlay(){}
	~GamePlay(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

