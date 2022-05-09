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
    public DX12Library::Scene
{
public:
    //�X�e�[�W��
    static const int STAGE_COUNT = 6;

private:
    //���C�g
    DX12Library::Light light;

    //�J����
    DX12Library::Camera camera;

    //�X�e�[�W�R���e�i
    std::vector<std::unique_ptr<Stage>> stages;

    //�J�����̈ʒu���X�g
    std::vector<DX12Library::Vector3> cameraPosList;

    //���ݑI������Ă���X�e�[�W�C���f�b�N�X
    int nowSelectStageIndex = 1;

    //�ړ�����������ǂ���
    bool isMoveUp = false;

    //�J���G�t�F�N�g�^�C�}�[
    DX12Library::Timer firstEffectTimer;

    //�I������Ă���X�e�[�W���ς�������ɊJ�n����^�C�}�[
    DX12Library::Timer changeSelectPosTimer;

    //�Q�[���J�n�G�t�F�N�g�^�C�}�[
    DX12Library::Timer startGameTimer;

    //�J��Ԃ��G�t�F�N�g�^�C�}�[
    DX12Library::Timer roopEffectTimer;

    //���G�t�F�N�g�^�C�}�[
    DX12Library::Timer arrowTimer;

    //UI ��{�^��
    UISquareButton buttonUp;

    //UI ���{�^��
    UISquareButton buttonDown;

    //UI START�{�^��
    UISquareButton buttonStart;

    //�w�i(������)
    DX12Library::Sprite sprStageBG;

    //�w�i(��)
    DX12Library::Sprite sprBackground;

    //�uStage�v����
    DX12Library::Sprite sprTextStage;

    //�X�e�[�W����
    DX12Library::Sprite sprStageNum[2];

    //���
    DX12Library::Sprite sprArrowUp;
    DX12Library::Sprite sprArrowDown;

    //�O�i(��)
    DX12Library::Sprite sprWrite;

    //�O�i(��)
    DX12Library::Sprite sprBlack;

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
    void UpdateButton();

    void DrawStage();
    void DrawUI();
    void DrawFG();
    void DrawStageNumTex();
};

