#pragma once
#include <Scene.h>
#include "Sprite.h"
#include "Object3D.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Particle3D.h"
#include <vector>

class Title :
    public Scene
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
        Object3D object;        //オブジェクト
        Vector3 position;       //位置
        Vector3 velocity;       //移動量
        float moveSpeed;        //移動スピード
        Vector3 addRotVelocity; //加える回転量
        Timer breakTimer;       //破壊までの時間を計測
    };

private:
    //ライト
    Light light;

    //カメラ
    Camera camera;

    //テキスト
    Sprite sprTextTitle;
    Sprite sprTextClick;
    //注意書き
    Sprite sprAttention;

    //前景(黒)
    Sprite sprBlack;
    //前景(白)
    Sprite sprWhite;

    //背景
    Object3D objBG;
    ObjModel modelBG;

    //開幕エフェクトタイマー
    Timer firstEffectTimer;

    //シーンチェンジタイマー
    Timer sceneChangeTimer;

    //「Click」文字透明度タイマー
    Timer clickAlphaTimer;

    //エフェクトに使用するオブジェクト配列
    std::vector<EffectObject*> effectObjects;

    //エフェクト追加タイマー
    Timer addEffectTimer;

    //パーティクル(四角)
    Particle3D particleSquare[5];
    //パーティクル(三角)
    Particle3D particleTriangle[5];
    //パーティクル(ごつごつ)
    Particle3D particleBreak;

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
    Vector3 GetRandomOnUnitSpherePos();
};

