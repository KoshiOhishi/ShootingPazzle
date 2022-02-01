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
#include "EditorStage.h"
#include "StartLane.h"

enum EditorMode {
	MODE_ADD,
	MODE_DELETE,
	MODE_OPTION
};

enum EditObjectType {
	OBJECTTYPE_BLOCK,
	OBJECTTYPE_FLOOR
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
	EditorStage stage;


	//Editor
	//モード
	int mode = MODE_ADD;
	int objectType = OBJECTTYPE_BLOCK;
	int blockType = BLOCKTYPE_SQUARE;
	int floorType = FLOORTYPE_NORMAL;
	//壊れるまでの衝突回数　0で壊れないブロック
	int breakupCount = 0;
	int blockColor = BLOCK_COLOR_NONE;
	//現在のカーソル位置
	StageVec2 nowCursolPos;
	//オプションでのモード
	int optionMode = OPTION_SET_STARTLANE;

	//表示用オブジェクト
	Object3D objDispBlock;
	Object3D objDispFloor;
	Object3D objDispFloorSub;	//補助オブジェクト
	StartLane startLane[2];

	//SaveLoad用名前格納
	std::string ioname;

	//文字入力中か
	bool isEnteringIOName = false;

	//ImGuiスライダーWidthDepth
	int sliderWidth;
	int sliderDepth;

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
	void UpdateDispObject();
	void UpdateStartLane();

	void DrawStartLane();
	void DrawBlock();
	void DrawFloor();

	void Save();

	void Load();

	void UpdateImgui();

	/// <summary>
	/// 現在のステージ上のカーソルの位置を取得
	/// </summary>
	void CalcNowCursolPos();

	/// <summary>
	/// ステージを再構築する
	/// </summary>
	void ReCreateStage(unsigned short width, unsigned short depth);
};