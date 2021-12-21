#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "Light.h"
#include "GameCamera.h"
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

class GamePlay :
	public Scene
{
private:
	//���C�g
	Light light;
	//�J����
	GameCamera camera;
	//�e
	MyBullet myBullet;
	//�X�e�[�W
	Stage stage;
	//�X�e�[�W�p�X
	std::string stagePass = ".. / StageEditor/StageData/test.spb";

	//�w�i
	Object3D objBG;
	ObjModel modelBG;

public:
	GamePlay(){}
	~GamePlay(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;

	/// <summary>
	/// �Ֆʂ����Z�b�g���A�t�F�[�Y��SET_POS�ɂ��ǂ�
	/// </summary>
	void Reset();
};

