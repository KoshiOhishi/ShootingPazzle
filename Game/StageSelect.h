#pragma once
#include <vector>
#include <string>
#include <Scene.h>
#include "Light.h"
#include "Camera.h"
#include "Stage.h"
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

public:
    StageSelect();
    ~StageSelect();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    void DrawStageData();
};

