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
#include "Particle2D.h"
#include "Particle3D.h"

#include "MyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "Stage.h"
#include "Tutorial.h"

class GamePlay :
	public DX12Library::Scene
{
private:
	//ライト
	DX12Library::Light light;
	//カメラ
	GameCamera camera;
	//弾
	MyBullet myBullet;

	//ステージ
	Stage stage;

	//背景
	DX12Library::Object3D objBG;
	DX12Library::ObjModel modelBG;

	//UIのブロック
	DX12Library::Object3D objUISquareBlock;

	//残りブロックの数字 (3桁まで対応)
	DX12Library::Sprite sprRemainingBlockCount[3];

	//前景(白)
	DX12Library::Sprite sprWhite;

	//前景(半透明黒)
	DX12Library::Sprite sprBlack;

	//残りブロック数のUI
	DX12Library::Sprite sprUIRemainingBlock;

	//ステージセレクトに戻るかのポップアップ
	DX12Library::Sprite sprPopUp;

	//「Clear」文字 (エフェクト用に2こ分)
	DX12Library::Sprite sprTextClear[2];

	//「ClearText」文字
	DX12Library::Sprite sprTextClearTime;

	//クリアタイムの数字 (9999.999sまで対応)
	DX12Library::Sprite sprTextTimeNumber[9];

	//UI(リセット)
	UISquareButton buttonReset;

	//UI(ステージセレクトに戻る)
	UISquareButton buttonBack;

	//UI(Yes)
	UISquareButton buttonYes;

	//UI(No)
	UISquareButton buttonNo;

	//UI(OK)
	UISquareButton buttonOK;

	//開幕エフェクトタイマー
	DX12Library::Timer firstEffectTimer;

	//ポップアップ出現タイマー
	DX12Library::Timer dispPopUpTimer;

	//シーンチェンジタイマー
	DX12Library::Timer sceneChangeTimer;

	//クリアエフェクトタイマー
	DX12Library::Timer clearEffectTimer;

	//スコアタイマー
	DX12Library::Timer scoreTimer;

	//パーティクル追加タイマー
	DX12Library::Timer addParticleTimer;

	//1回目の開幕エフェクト終了フラグ
	bool isEndFirstEffectOnce = false;

	//ポップアップ表示フラグ
	bool isDispPopup = false;

	//パーティクル
	DX12Library::Particle2D particle[2];

	//クリアエフェクトのパーティクルが追加されたか
	bool addedParticleClearEffect = false;

	//チュートリアルオブジェクト
	Tutorial tutorial;

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
	void UpdateChangeScene();

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

	/// <summary>
	/// 残りブロックカウントの数字画像準備
	/// </summary>
	/// <param name="pNumberTexArray"></param>
	/// <param name="arraySize"></param>
	/// <param name="drawNum"></param>
	void SetRemainingBlockCountTex(DX12Library::Sprite* pNumberTexArray, int arraySize, int drawNum, float numWidth, float numHeight);

	/// <summary>
	/// 残りブロック数の座標一括セット
	/// </summary>
	/// <param name="leftUpPos"></param>
	void SetRemainingBlockCountTexPos(DX12Library::Sprite* pNumberTexArray, int arraySizeconst, const DX12Library::Vector2& leftUpPos, float padding);

	/// <summary>
	/// スコアタイムのスプライトを準備する
	/// </summary>
	/// <param name="pNumberTexArray"></param>
	/// <param name="arraySize"></param>
	/// <param name="startDrawTime"></param>
	void SetScoreTimeTex(DX12Library::Sprite* pNumberTexArray, int arraySize, int startDrawTime);

	/// <summary>
	/// スコアタイムをketaで丸めて取得
	/// </summary>
	/// <returns></returns>
	std::string GetStrScoreTime(int keta);
};

