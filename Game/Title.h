#pragma once
#include <Scene.h>
#include "Sprite.h"
#include "Object3D.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Particle3D.h"
#include <vector>
#include <memory>

class Title :
    public DX12Library::Scene
{
    static const float EFFECT_ACCEL;

    enum ParticleType {
        PARTICLE_TYPE_SQUARE,
        PARTICLE_TYPE_TRIANGLE,
        PARTICLE_TYPE_BREAK,
    };

    struct EffectObject
    {
        int type = -1;          //パーティクルタイプ
        int colorType = -1;     //色
        DX12Library::Object3D object;        //オブジェクト
        DX12Library::Vector3 position;       //位置
        DX12Library::Vector3 velocity;       //移動量
        float moveSpeed;        //移動スピード
        DX12Library::Vector3 addRotVelocity; //加える回転量
        DX12Library::Timer breakTimer;       //破壊までの時間を計測
    };

private:
    //ライト
    DX12Library::Light light;

    //カメラ
    DX12Library::Camera camera;

    //テキスト
    DX12Library::Sprite sprTextTitle;
    DX12Library::Sprite sprTextClick;
    //注意書き
    DX12Library::Sprite sprAttention;

    //前景(黒)
    DX12Library::Sprite sprBlack;
    //前景(白)
    DX12Library::Sprite sprWhite;

    //背景
    DX12Library::Object3D objBG;
    DX12Library::ObjModel modelBG;

    //開幕エフェクトタイマー
    DX12Library::Timer firstEffectTimer;

    //シーンチェンジタイマー
    DX12Library::Timer sceneChangeTimer;

    //「Click」文字透明度タイマー
    DX12Library::Timer clickAlphaTimer;

    //エフェクトに使用するオブジェクト配列
    std::vector<std::unique_ptr<EffectObject>> effectObjects;

    //エフェクト追加タイマー
    DX12Library::Timer addEffectTimer;

    //パーティクル(四角)
    DX12Library::Particle3D particleSquare[5];
    //パーティクル(三角)
    DX12Library::Particle3D particleTriangle[5];
    //パーティクル(ごつごつ)
    DX12Library::Particle3D particleBreak;

public:

    Title();
    ~Title();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    //各種更新
    void UpdateInput();
    void UpdateTimer();
    void UpdateTextTex();
    void UpdateAttention();
    void UpdateBG();
    void UpdateFG();
    void UpdateEffectObjects();
    void UpdateSound();

    //各種描画
    void DrawTextTex();
    void DrawAttention();
    void DrawBG();
    void DrawFG();
    void DrawEffectObjects();

    //エフェクトオブジェクト追加
    void AddEffectObject();

    //パーティクル追加
    void AddParticle(const EffectObject& effectObject);

    //タイマーセット
    void SetAndStartAddEffectTimer();

    /// <summary>
    /// float型乱数取得
    /// </summary>
    /// <param name="min">最小値</param>
    /// <param name="max">最大値</param>
    /// <returns></returns>
    float GetRandF(int min, int max);

    /// <summary>
    /// 単位球上のランダムな位置を取得
    /// </summary>
    DX12Library::Vector3 GetRandomOnUnitSpherePos();
};

