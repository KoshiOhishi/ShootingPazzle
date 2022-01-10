#pragma once
#include <vector>
#include <string>
#include <Scene.h>
#include "Light.h"
#include "Camera.h"
#include "Stage.h"
#include "UISquareButton.h"
#include "Sprite.h"

class StageSelect :
    public Scene
{
public:
    //ステージ数
    static const int STAGE_COUNT = 4;
    static const std::string STAGE_DIRECTORY;

private:
    //ライト
    Light light;

    //カメラ
    Camera camera;

    //ステージコンテナ
    std::vector<Stage*> stages;

    //現在選択されているステージインデックス
    int nowSelectStageIndex = 0;

    //移動が上向きかどうか
    bool isMoveUp = false;

    //選択されているステージが変わった時に開始するタイマー
    Timer changeSelectPosTimer;

    //ゲーム開始エフェクトタイマー
    Timer startGameTimer;

    //UI 上ボタン
    UISquareButton buttonUp;

    //UI 下ボタン
    UISquareButton buttonDown;

    //UI STARTボタン
    UISquareButton buttonStart;

    //背景(白い丸)
    Sprite sprStageBG;

    //背景(黒)
    Sprite sprBackground;

    //前景(白)
    Sprite sprWriteAll;


public:
    StageSelect();
    ~StageSelect();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    void UpdateCamera();
    void UpdateNowSelect();
    void UpdateStage();
    void UpdateAfterDecided();

    void DrawStage();
    void DrawUI();
    void DrawWrite();
};

