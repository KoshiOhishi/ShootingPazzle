#pragma once
#include <vector>
#include <string>
#include <Scene.h>
#include "Light.h"
#include "Camera.h"
#include "Stage.h"
#include "UISquareButton.h"
#include "Sprite.h"
#include "Particle2D.h"

class StageSelect :
    public DX12Library::Scene
{
public:
    //ステージ数
    static const int STAGE_COUNT = 6;

private:
    //ライト
    DX12Library::Light light;

    //カメラ
    DX12Library::Camera camera;

    //ステージコンテナ
    std::vector<std::unique_ptr<Stage>> stages;

    //カメラの位置リスト
    std::vector<DX12Library::Vector3> cameraPosList;

    //現在選択されているステージインデックス
    int nowSelectStageIndex = 1;

    //移動が上向きかどうか
    bool isMoveUp = false;

    //開幕エフェクトタイマー
    DX12Library::Timer firstEffectTimer;

    //選択されているステージが変わった時に開始するタイマー
    DX12Library::Timer changeSelectPosTimer;

    //ゲーム開始エフェクトタイマー
    DX12Library::Timer startGameTimer;

    //繰り返しエフェクトタイマー
    DX12Library::Timer roopEffectTimer;

    //矢印エフェクトタイマー
    DX12Library::Timer arrowTimer;

    //UI 上ボタン
    UISquareButton buttonUp;

    //UI 下ボタン
    UISquareButton buttonDown;

    //UI STARTボタン
    UISquareButton buttonStart;

    //背景(白い丸)
    DX12Library::Sprite sprStageBG;

    //背景(黒)
    DX12Library::Sprite sprBackground;

    //「Stage」文字
    DX12Library::Sprite sprTextStage;

    //ステージ数字
    DX12Library::Sprite sprStageNum[2];

    //矢印
    DX12Library::Sprite sprArrowUp;
    DX12Library::Sprite sprArrowDown;

    //前景(白)
    DX12Library::Sprite sprWrite;

    //前景(黒)
    DX12Library::Sprite sprBlack;

public:
    StageSelect();
    ~StageSelect();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    void UpdateCamera();
    void UpdateTimer();
    void UpdateNowSelect();
    void UpdateStage();
    void UpdateAfterDecided();
    void UpdateStageNumTex();
    void UpdateButton();

    void DrawStage();
    void DrawUI();
    void DrawFG();
    void DrawStageNumTex();
};

