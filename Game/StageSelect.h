#pragma once
#include <vector>
#include <string>
#include <Scene.h>
#include "Light.h"
#include "Camera.h"
#include "Stage.h"
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

public:
    StageSelect();
    ~StageSelect();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    void DrawStageData();
};

