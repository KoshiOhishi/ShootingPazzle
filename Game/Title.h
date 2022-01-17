#pragma once
#include <Scene.h>
#include "Sprite.h"
#include "Object3D.h"
#include "ObjModel.h"
#include "Timer.h"

class Title :
    public Scene
{
private:
    //ライト
    Light light;

    //カメラ
    Camera camera;

    //テキスト
    Sprite sprTextTitle;
    Sprite sprTextClick;

    //前景(黒)
    Sprite sprBlack;

    //背景
    Object3D objBG;
    ObjModel modelBG;

    //開幕エフェクトタイマー
    Timer firstEffectTimer;

    //シーンチェンジタイマー
    Timer sceneChangeTimer;

    //「Click」文字透明度タイマー
    Timer clickAlphaTimer;

public:

    Title();
    ~Title();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    void UpdateInput();
    void UpdateTimer();
    void UpdateTextTex();
    void UpdateBG();
    void UpdateFG();

    void DrawTextTex();
    void DrawBG();
    void DrawFG();
};

