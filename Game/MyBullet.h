#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "BaseBlock.h"
#include "CollisionManager.h"
#include "Stage.h"
#include "ParticleManager.h"
#include "Sound.h"

const int MYBULLET_START_FIRST_EFFECT_TIME = 2500;
const int MYBULLET_END_FIRST_EFFECT_TIME = 3500;

class MyBullet
{
private:
	//�ǂȂǂɏՓ˂����Ƃ��ɎQ�Ƃ���\����
	struct NextMoveInfo {
		//���t���[���̈ʒu
		DX12Library::Vector3 nextPos = {};
		//���t���[���̈ړ���
		DX12Library::Vector3 nextVel = {};
		//���t���[���ɏՓ˂��邩
		bool isCollisionNextFrame = false;
	};

private:
	//���a
	static const float RADIUS;

	//�����f��
	static DX12Library::ObjModel modelSphere;
	//��󃂃f��
	static DX12Library::ObjModel modelArrow;

	//���W
	DX12Library::Vector3 position = {};
	//�ړ���
	DX12Library::Vector3 velocity = {};
	//�ړ����x
	float speed = 2.0f;
	//�ˏo�p�x
	float shotAngle = 0;
	//���C
	float friction = 0.0005f;
	//�d��
	float gravity = 0;
	//�ˏo�t���O
	bool isShoot = false;
	//���C
	DX12Library::Ray ray;
	//�Փ˃t���[���̈ʒu��ړ��ʂ��܂Ƃ߂��\����
	NextMoveInfo nextMoveInfo;
	//�X�e�[�W�f�[�^
	Stage* stage = nullptr;
	//���I�u�W�F�N�g
	DX12Library::Object3D objSphere;
	//���I�u�W�F�N�g
	DX12Library::Object3D objArrow;

	//�G�t�F�N�g�^�C�}�[
	DX12Library::Timer* pFirstEffectTimer;
	//���˂�G�t�F�N�g�̏���Y���W �J�����̈ʒu�Ɠ���������
	float bounceInitPosY = 200;

	//�F�ύX�^�C�}�[
	DX12Library::Timer changeColorTimer;
	//�F�ύX�Ɏg�p����ϐ� (0...�O�̐F 1...���̐F)
	int myColor[2] = { STAGE_COLOR_NONE, STAGE_COLOR_NONE };

public:
	static void StaticInitialize();

public:
	MyBullet();

	~MyBullet();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �o���G�t�F�N�g���̍X�V
	/// </summary>
	void UpdateFirstEffect();

	/// <summary>
	/// �N���A�G�t�F�N�g�X�V
	/// </summary>
	void UpdateClearEffect();

	/// <summary>
	/// �ˏo�O�̍X�V
	/// </summary>
	void UpdateBeforeShoot();

	/// <summary>
	/// �e�����ʒu�����߂�
	/// </summary>
	void DecideShootPos();

	/// <summary>
	/// �e�����p�x�����߂�
	/// </summary>
	float DecideShootAngle();

	/// <summary>
	/// �e���Z�b�g���ꂽ�p�x�Ŏˏo
	/// </summary>
	void Shoot();

	/// <summary>
	/// �e�𓮂���
	/// </summary>
	void Move();

	/// <summary>
	/// ���C��K�p
	/// </summary>
	void ApplyFriction();

	/// <summary>
	/// �d�͂�K�p
	/// </summary>
	void ApplyGravity();

	/// <summary>
	/// �Փ˃`�F�b�N
	/// </summary>
	void CheckCollision();

	/// <summary>
	/// �u���b�N�Ƃ̏Փ˃`�F�b�N
	/// </summary>
	void CheckBlockCollision();

	/// <summary>
	/// ���Ƃ̏Փ˃`�F�b�N
	/// </summary>
	void CheckFloorCollision();

	/// <summary>
	/// ���C�̈ʒu���p�X�V
	/// </summary>
	void UpdateRay(const DX12Library::Vector3& pos, const DX12Library::Vector3& dir);

	/// <summary>
	/// �����̈ʒu���X�e�[�W�̊O�ɂ��邩
	/// </summary>
	/// <returns></returns>
	bool IsOutStage(const DX12Library::Vector3& pos);

	/// <summary>
	/// �F��ύX����
	/// </summary>
	/// <param name="newColor"></param>
	void ChangeColor(int newColor);

	/// <summary>
	/// �F�X�V
	/// </summary>
	void UpdateColor();
#pragma region Setter

	/// <summary>
	/// �X�e�[�W�|�C���^�Z�b�g
	/// </summary>
	/// <param name="blocks"></param>
	void SetPStage(Stage* stage) { this->stage = stage; }

	/// <summary>
	/// ���˂�G�t�F�N�g�̏���Y���W�Z�b�g
	/// </summary>
	/// <param name="y"> ���˂�G�t�F�N�g�̏���Y���W</param>
	void SetBounceInitPosY(float y) { bounceInitPosY = y; }

	void SetPFirstEffectTimer(DX12Library::Timer* pFirstEffectTimer) { this->pFirstEffectTimer = pFirstEffectTimer; }
#pragma endregion

#pragma region Getter

#pragma endregion
};

