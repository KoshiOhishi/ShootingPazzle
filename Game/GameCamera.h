#pragma once
#include <Camera.h>
#include "GameUtility.h"
#include "Timer.h"

class GameCamera :
    public Camera
{
private:
    //出現エフェクト
    Timer firstEffectTimer;

    //エフェクト前位置
    Vector3 beforeEffectPos;
    //エフェクト後位置
    Vector3 afterEffectPos;
    //エフェクト前回転量
    Vector3 beforeEffectRot;
    //エフェクト後回転量
    Vector3 afterEffectRot;

public:
    void Initialize() override;

    void Update() override;

    /// <summary>
    /// 出現エフェクト
    /// </summary>
    void UpdateFirseEffect();

    /// <summary>
    /// ステージの大きさからカメラ位置をセット
    /// </summary>
    /// <param name="stageSize">ステージの大きさ</param>
    void SetPosFromStageSize(const StageVec2& stageSize);
};

