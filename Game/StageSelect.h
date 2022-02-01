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
    public Scene
{
public:
    //�X�e�[�W��
    static const int STAGE_COUNT = 6;
    static const std::string STAGE_DIRECTORY;

private:
    //���C�g
    Light light;

    //�J����
    Camera camera;

    //�X�e�[�W�R���e�i
    std::vector<Stage*> stages;

    //���ݑI������Ă���X�e�[�W�C���f�b�N�X
    int nowSelectStageIndex = 1;

    //�ړ�����������ǂ���
    bool isMoveUp = false;

    //�J���G�t�F�N�g�^�C�}�[
    Timer firstEffectTimer;

    //�I������Ă���X�e�[�W���ς�������ɊJ�n����^�C�}�[
    Timer changeSelectPosTimer;

    //�Q�[���J�n�G�t�F�N�g�^�C�}�[
    Timer startGameTimer;

    //�J��Ԃ��G�t�F�N�g�^�C�}�[
    Timer roopEffectTimer;

    //���G�t�F�N�g�^�C�}�[
    Timer arrowTimer;

    //UI ��{�^��
    UISquareButton buttonUp;

    //UI ���{�^��
    UISquareButton buttonDown;

    //UI START�{�^��
    UISquareButton buttonStart;

    //�w�i(������)
    Sprite sprStageBG;

    //�w�i(��)
    Sprite sprBackground;

    //�uStage�v����
    Sprite sprTextStage;

    //�X�e�[�W����
    Sprite sprStageNum[2];

    //�O�i(��)
    Sprite sprWrite;

    //�O�i(��)
    Sprite sprBlack;

    //�ړ��\�Ȃ��Ƃ�\�����I�u�W�F�N�g
    ObjModel modelArrowUp;
    ObjModel modelArrowDown;
    Object3D objArrowUp;
    Object3D objArrowDown;


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

    void DrawStage();
    void DrawUI();
    void DrawFG();
    void DrawStageNumTex();
};

