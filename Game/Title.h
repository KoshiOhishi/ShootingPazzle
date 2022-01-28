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
    //���C�g
    Light light;

    //�J����
    Camera camera;

    //�e�L�X�g
    Sprite sprTextTitle;
    Sprite sprTextClick;
    //���ӏ���
    Sprite sprAttention;

    //�O�i(��)
    Sprite sprBlack;
    //�O�i(��)
    Sprite sprWhite;

    //�w�i
    Object3D objBG;
    ObjModel modelBG;

    //�J���G�t�F�N�g�^�C�}�[
    Timer firstEffectTimer;

    //�V�[���`�F���W�^�C�}�[
    Timer sceneChangeTimer;

    //�uClick�v���������x�^�C�}�[
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
    void UpdateAttention();
    void UpdateBG();
    void UpdateFG();

    void DrawTextTex();
    void DrawAttention();
    void DrawBG();
    void DrawFG();
};

