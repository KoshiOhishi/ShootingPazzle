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
#include "StartLane.h"

enum EditorMode {
	MODE_ADD,
	MODE_DELETE,
	MODE_OPTION
};

enum OptionMode {
	OPTION_SET_STARTLANE
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
	StageVec2 nowCursolPos;
	//オプションでのモード
	int optionMode = OPTION_SET_STARTLANE;

	//表示用オブジェクト
	SquareBlock squareBlock;
	TriangleBlock triangleBlock[4];
	StartLane startLane[2];

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
	void UpdateStartLane();

	void DrawEdit();

	void DrawStartLane();
	void DrawBlock();

	void Save();

	void Load();

	void UpdateImgui();

	/// <summary>
	/// 現在のステージ上のカーソルの位置を取得
	/// </summary>
	void CalcNowCursolPos();
};