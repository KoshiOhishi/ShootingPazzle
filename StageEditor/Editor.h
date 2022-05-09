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
	OPTION_CHANGE_STAGESIZE,
	OPTION_SET_STARTLANE,
	OPTION_SAVE,
	OPTION_LOAD
};

class Editor :
	public DX12Library::Scene
{
private:
	//ライト
	DX12Library::Light light;
	//カメラ
	DX12Library::Camera camera;
	//タイマー
	DX12Library::Timer timer;
	//ステージ
	EditorStage stage;

	//背景
	DX12Library::Object3D objBG;
	DX12Library::ObjModel modelBG;

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
	int optionMode = OPTION_CHANGE_STAGESIZE;

	//表示用オブジェクト
	DX12Library::Object3D objDispBlock;
	DX12Library::Object3D objDispFloor;
	DX12Library::Object3D objDispFloorSub;	//補助オブジェクト

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
	DX12Library::RenderText txtCursol;

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