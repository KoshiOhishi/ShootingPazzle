#pragma once
#include <Scene.h>
#include "Sprite.h"
#include "Object3D.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Particle3D.h"
#include <vector>
#include <memory>

class Title :
    public DX12Library::Scene
{
    static const float EFFECT_ACCEL;

    enum ParticleType {
        PARTICLE_TYPE_SQUARE,
        PARTICLE_TYPE_TRIANGLE,
        PARTICLE_TYPE_BREAK,
    };

    struct EffectObject
    {
        int type = -1;          //�p�[�e�B�N���^�C�v
        int colorType = -1;     //�F
        DX12Library::Object3D object;        //�I�u�W�F�N�g
        DX12Library::Vector3 position;       //�ʒu
        DX12Library::Vector3 velocity;       //�ړ���
        float moveSpeed;        //�ړ��X�s�[�h
        DX12Library::Vector3 addRotVelocity; //�������]��
        DX12Library::Timer breakTimer;       //�j��܂ł̎��Ԃ��v��
    };

private:
    //���C�g
    DX12Library::Light light;

    //�J����
    DX12Library::Camera camera;

    //�e�L�X�g
    DX12Library::Sprite sprTextTitle;
    DX12Library::Sprite sprTextClick;
    //���ӏ���
    DX12Library::Sprite sprAttention;

    //�O�i(��)
    DX12Library::Sprite sprBlack;
    //�O�i(��)
    DX12Library::Sprite sprWhite;

    //�w�i
    DX12Library::Object3D objBG;
    DX12Library::ObjModel modelBG;

    //�J���G�t�F�N�g�^�C�}�[
    DX12Library::Timer firstEffectTimer;

    //�V�[���`�F���W�^�C�}�[
    DX12Library::Timer sceneChangeTimer;

    //�uClick�v���������x�^�C�}�[
    DX12Library::Timer clickAlphaTimer;

    //�G�t�F�N�g�Ɏg�p����I�u�W�F�N�g�z��
    std::vector<std::unique_ptr<EffectObject>> effectObjects;

    //�G�t�F�N�g�ǉ��^�C�}�[
    DX12Library::Timer addEffectTimer;

    //�p�[�e�B�N��(�l�p)
    DX12Library::Particle3D particleSquare[5];
    //�p�[�e�B�N��(�O�p)
    DX12Library::Particle3D particleTriangle[5];
    //�p�[�e�B�N��(������)
    DX12Library::Particle3D particleBreak;

public:

    Title();
    ~Title();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;

    //�e��X�V
    void UpdateInput();
    void UpdateTimer();
    void UpdateTextTex();
    void UpdateAttention();
    void UpdateBG();
    void UpdateFG();
    void UpdateEffectObjects();
    void UpdateSound();

    //�e��`��
    void DrawTextTex();
    void DrawAttention();
    void DrawBG();
    void DrawFG();
    void DrawEffectObjects();

    //�G�t�F�N�g�I�u�W�F�N�g�ǉ�
    void AddEffectObject();

    //�p�[�e�B�N���ǉ�
    void AddParticle(const EffectObject& effectObject);

    //�^�C�}�[�Z�b�g
    void SetAndStartAddEffectTimer();

    /// <summary>
    /// float�^�����擾
    /// </summary>
    /// <param name="min">�ŏ��l</param>
    /// <param name="max">�ő�l</param>
    /// <returns></returns>
    float GetRandF(int min, int max);

    /// <summary>
    /// �P�ʋ���̃����_���Ȉʒu���擾
    /// </summary>
    DX12Library::Vector3 GetRandomOnUnitSpherePos();
};

