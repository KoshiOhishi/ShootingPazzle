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

    //�O�i(��)
    Sprite sprBlack;

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
    void UpdateBG();
    void UpdateFG();

    void DrawTextTex();
    void DrawBG();
    void DrawFG();
};

