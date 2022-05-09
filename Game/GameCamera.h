#pragma once
#include <Camera.h>
#include "GameUtility.h"
#include "Timer.h"

class GameCamera :
    public DX12Library::Camera
{
private:
    //�o���G�t�F�N�g�^�C�}�[�̃|�C���^
    DX12Library::Timer* pFirstEffectTimer;
    //�N���A�G�t�F�N�g�^�C�}�[�̃|�C���^
    DX12Library::Timer* pClearEffectTimer;

    //�G�t�F�N�g�O�ʒu
    DX12Library::Vector3 beforeFirstEffectPos;
    //�G�t�F�N�g��ʒu
    DX12Library::Vector3 afterFirstEffectPos;
    //�G�t�F�N�g�O��]��
    DX12Library::Vector3 beforeFirstEffectRot;
    //�G�t�F�N�g���]��
    DX12Library::Vector3 afterFirstEffectRot;

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

    void SetPFirstEffectTimer(DX12Library::Timer* pFirstEffectTimer) { this->pFirstEffectTimer = pFirstEffectTimer; }
    void SetPClearEffectTimer(DX12Library::Timer* pClearEffectTimer) { this->pClearEffectTimer = pClearEffectTimer; }
};

