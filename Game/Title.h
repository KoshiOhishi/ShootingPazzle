#pragma once
#include <Scene.h>
#include "Sprite.h"
#include "Object3D.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Particle3D.h"
#include <vector>

class Title :
    public Scene
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
        Object3D object;        //�I�u�W�F�N�g
        Vector3 position;       //�ʒu
        Vector3 velocity;       //�ړ���
        float moveSpeed;        //�ړ��X�s�[�h
        Vector3 addRotVelocity; //�������]��
        Timer breakTimer;       //�j��܂ł̎��Ԃ��v��
    };

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

    //�G�t�F�N�g�Ɏg�p����I�u�W�F�N�g�z��
    std::vector<EffectObject*> effectObjects;

    //�G�t�F�N�g�ǉ��^�C�}�[
    Timer addEffectTimer;

    //�p�[�e�B�N��(�l�p)
    Particle3D particleSquare[5];
    //�p�[�e�B�N��(�O�p)
    Particle3D particleTriangle[5];
    //�p�[�e�B�N��(������)
    Particle3D particleBreak;

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
    Vector3 GetRandomOnUnitSpherePos();
};

