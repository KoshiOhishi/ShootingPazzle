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

#include "Command.h"

//モード
enum EditorMode {
	MODE_ADD,
	MODE_DELETE,
	MODE_OPTION
};

//オプションモード
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

	//背景
	Object3D objBG;
	ObjModel modelBG;

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

	//スタートレーン表示用
	StartLane startLane[2];

	//SaveLoad用名前格納
	std::string ioname;

	//文字入力中か
	bool isEnteringIOName = false;

	//Stage再構築用前フレームステージサイズ
	StageVec2 prevSize;

	//ImGuiスライダーWidthDepth
	int sliderWidth;
	int sliderDepth;

	//カーソル位置表示
	RenderText txtCursol;

public:
	Editor();
	~Editor(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void UpdateCamera();
	void UpdateEdit();

	void UpdateAdd();
	void UpdateDelete();
	void UpdateOption();
	void UpdateObject();
	void UpdateStage();
	void UpdateDispObject();
	void UpdateStartLane();

	void DrawStartLane();
	void DrawDispBlock();
	void DrawDispFloor();

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
	void ReCreateStage(const StageVec2& prevSize, const StageVec2& nowSize);

	bool IsInsideCursol();
};