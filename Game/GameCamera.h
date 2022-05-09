#pragma once
#include <Camera.h>
#include "GameUtility.h"
#include "Timer.h"

class GameCamera :
    public DX12Library::Camera
{
private:
    //出現エフェクトタイマーのポインタ
    DX12Library::Timer* pFirstEffectTimer;
    //クリアエフェクトタイマーのポインタ
    DX12Library::Timer* pClearEffectTimer;

    //エフェクト前位置
    DX12Library::Vector3 beforeFirstEffectPos;
    //エフェクト後位置
    DX12Library::Vector3 afterFirstEffectPos;
    //エフェクト前回転量
    DX12Library::Vector3 beforeFirstEffectRot;
    //エフェクト後回転量
    DX12Library::Vector3 afterFirstEffectRot;

public:
    void Initialize() override;

    void Update() override;

    /// <summary>
    /// 出現エフェクト
    /// </summary>
    void UpdateFirstEffect();

    /// <summary>
    /// クリアエフェクト
    /// </summary>
    void UpdateClearEffect();

    /// <summary>
    /// 射出後のパラメータでカメラセット
    /// </summary>
    void SetCameraParamAfterShoot();

    /// <summary>
    /// ステージの大きさからカメラ位置をセット
    /// </summary>
    /// <param name="stageSize">ステージの大きさ</param>
    /// <returns>セットされたY座標</returns>
    float SetPosFromStageSize(const StageVec2& stageSize);

    void SetPFirstEffectTimer(DX12Library::Timer* pFirstEffectTimer) { this->pFirstEffectTimer = pFirstEffectTimer; }
    void SetPClearEffectTimer(DX12Library::Timer* pClearEffectTimer) { this->pClearEffectTimer = pClearEffectTimer; }
};

