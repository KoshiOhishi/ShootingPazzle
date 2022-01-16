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
#include "UISquareButton.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "Stage.h"

class GamePlay :
	public Scene
{
private:
	//ライト
	Light light;
	//カメラ
	GameCamera camera;
	//弾
	MyBullet myBullet;

	//ステージ
	Stage stage;

	//背景
	Object3D objBG;
	ObjModel modelBG;

	//前景(白)
	Sprite sprWhite;

	//前景(半透明黒)
	Sprite sprBlack;

	//ステージセレクトに戻るかのポップアップ
	Sprite sprPopUp;

	//「Clear」文字 (エフェクト用に2こ分)
	Sprite sprClearText[2];

	//UI(リセット)
	UISquareButton buttonReset;

	//UI(ステージセレクトに戻る)
	UISquareButton buttonBack;

	//UI(Yes)
	UISquareButton buttonYes;

	//UI(No)
	UISquareButton buttonNo;

	//開幕エフェクトタイマー
	Timer firstEffectTimer;

	//ポップアップ出現タイマー
	Timer dispPopUpTimer;

	//シーンチェンジタイマー
	Timer sceneChangeTimer;

	//クリアエフェクトタイマー
	Timer clearEffectTimer;

	//1回目の開幕エフェクト終了フラグ
	bool isEndFirstEffectOnce = false;

	//ポップアップ表示フラグ
	bool isDispPopup = false;


public:
	GamePlay();
	~GamePlay();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

	void UpdateDebugCamera();
	void UpdateTimer();
	void UpdateFirstEffect();
	void UpdateBG();
	void UpdateImgui();
	void UpdateUI();
	void UpdateStageBackPopUp();
	void UpdateClearEffect();

	void DrawWhiteEffect();
	void DrawBlackEffect();
	void DrawUI();
	void DrawStageBackPopUp();
	void DrawClearEffect();

	/// <summary>
	/// 盤面をリセットし、フェーズをSET_POSにもどす
	/// </summary>
	void Reset();

	/// <summary>
	/// クリアしているかチェック クリアしていたらフェーズをCLEARへ
	/// </summary>
	void CheckIsClear();
};

