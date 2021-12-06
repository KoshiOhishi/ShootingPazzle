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

enum EditorMode {
	MODE_ADD,
	MODE_DELETE
};

class Editor :
	public Scene
{
private:
	//ライト
	Light light;
	//カメラ
	Camera camera;
	//タイマー
	Timer timer;
	//ステージ
	Stage stage;


	//Editor
	//モード
	int mode = MODE_ADD;
	int blockType = BLOCKTYPE_SQUARE;
	int shapeType = SHAPETYPE_NO_LEFTTOP;
	//現在のカーソル位置
	StagePos nowCursolPos;

	//表示用オブジェクト
	SquareBlock squareBlock;
	TriangleBlock triangleBlock[4];

	//SaveLoad用名前格納
	std::string ioname;

	//文字入力中か
	bool isEnteringIOName = false;

public:
	Editor(){}
	~Editor(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void UpdateEdit();

	void UpdateAdd();
	void UpdateDelete();
	void UpdateObject();

	void DrawEdit();

	void DrawBlock();

	void Save();

	void Load();

	void UpdateImgui();

	/// <summary>
	/// 現在のステージ上のカーソルの位置を取得
	/// </summary>
	void CalcNowCursolPos();
};