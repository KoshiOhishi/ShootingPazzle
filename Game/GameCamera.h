#pragma once
#include <Camera.h>
#include "GameUtility.h"
#include "Timer.h"

class GameCamera :
    public Camera
{
private:
    //�o���G�t�F�N�g
    Timer effectTimer;

    //�G�t�F�N�g�O�ʒu
    Vector3 beforeFirstEffectPos;
    //�G�t�F�N�g��ʒu
    Vector3 afterFirstEffectPos;
    //�G�t�F�N�g�O��]��
    Vector3 beforeFirstEffectRot;
    //�G�t�F�N�g���]��
    Vector3 afterFirstEffectRot;

public:
    void Initialize() override;

    void Update() override;

    /// <summary>
    /// �o���G�t�F�N�g
    /// </summary>
    void UpdateFirstEffect();

    /// <summary>
    /// �N���A�G�t�F�N�g
    /// </summary>
    void UpdateClearEffect();

    /// <summary>
    /// �ˏo��̃p�����[�^�ŃJ�����Z�b�g
    /// </summary>
    void SetCameraParamAfterShoot();

    /// <summary>
    /// �X�e�[�W�̑傫������J�����ʒu���Z�b�g
    /// </summary>
    /// <param name="stageSize">�X�e�[�W�̑傫��</param>
    /// <returns>�Z�b�g���ꂽY���W</returns>
    float SetPosFromStageSize(const StageVec2& stageSize);

    /// <summary>
    /// �G�t�F�N�g�^�C�}�[���X�^�[�g������
    /// </summary>
    /// <param name="start"></param>
    /// <param name="end"></param>
    /// <param name="speed"></param>
    void StartEffectTimer(int start, int end, float speed = 1.0);
};

