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
#include "EnemyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "Stage.h"

#define PI 3.14159265359f

class Editor :
	public Scene
{
private:
	//���C�g
	Light light;
	//�J����
	Camera camera;
	//�^�C�}�[
	Timer timer;
	//�e
	MyBullet myBullet;
	//�X�e�[�W
	Stage stage;

public:
	Editor(){}
	~Editor(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

