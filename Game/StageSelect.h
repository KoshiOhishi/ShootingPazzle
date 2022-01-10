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
    //�X�e�[�W��
    static const int STAGE_COUNT = 4;
    static const std::string STAGE_DIRECTORY;

private:
    //���C�g
    Light light;

    //�J����
    Camera camera;

    //�X�e�[�W�R���e�i
    std::vector<Stage*> stages;

    //���ݑI������Ă���X�e�[�W�C���f�b�N�X
    int nowSelectStageIndex = 0;

    //�ړ�����������ǂ���
    bool isMoveUp = false;

    //�I������Ă���X�e�[�W���ς�������ɊJ�n����^�C�}�[
    Timer changeSelectPosTimer;

    //�Q�[���J�n�G�t�F�N�g�^�C�}�[
    Timer startGameTimer;

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

    //�O�i(��)
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

