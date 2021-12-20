#pragma once
#include <Camera.h>
#include "GameUtility.h"
#include "Timer.h"

class GameCamera :
    public Camera
{
private:
    //�o���G�t�F�N�g
    Timer firstEffectTimer;

    //�G�t�F�N�g�O�ʒu
    Vector3 beforeEffectPos;
    //�G�t�F�N�g��ʒu
    Vector3 afterEffectPos;
    //�G�t�F�N�g�O��]��
    Vector3 beforeEffectRot;
    //�G�t�F�N�g���]��
    Vector3 afterEffectRot;

public:
    void Initialize() override;

    void Update() override;

    /// <summary>
    /// �o���G�t�F�N�g
    /// </summary>
    void UpdateFirseEffect();

    /// <summary>
    /// �X�e�[�W�̑傫������J�����ʒu���Z�b�g
    /// </summary>
    /// <param name="stageSize">�X�e�[�W�̑傫��</param>
    void SetPosFromStageSize(const StageVec2& stageSize);
};

