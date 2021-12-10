#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "Block.h"
#include "CollisionManager.h"
#include "Stage.h"

class MyBullet
{
private:
	//�ǂȂǂɏՓ˂����Ƃ��ɎQ�Ƃ���\����
	struct NextMoveInfo {
		//���t���[���̈ʒu
		Vector3 nextPos = {};
		//���t���[���̈ړ���
		Vector3 nextVel = {};
		//���t���[���ɏՓ˂��邩
		bool isCollisionNextFrame = false;
	};

private:
	//�����f��
	static ObjModel modelSphere;
	//��󃂃f��
	static ObjModel modelArrow;

	//���W
	Vector3 position = {};
	//�ړ���
	Vector3 velocity = {};
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
	Ray ray;
	//�Փ˃t���[���̈ʒu��ړ��ʂ��܂Ƃ߂��\����
	NextMoveInfo nextMoveInfo;
	//�X�e�[�W�f�[�^
	Stage* stage = nullptr;
	//���I�u�W�F�N�g
	Object3D objSphere;
	//���I�u�W�F�N�g
	Object3D objArrow;

public:
	static void CreateModel();

public:
	MyBullet(){}

	~MyBullet(){}

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
	/// �Փ˃`�F�b�N
	/// </summary>
	void CheckCollision();

	/// <summary>
	/// ���C�̈ʒu���p�X�V
	/// </summary>
	void UpdateRay();

	/// <summary>
	/// �����̈ʒu���X�e�[�W�̊O�ɂ��邩
	/// </summary>
	/// <returns></returns>
	bool IsOutStage(const Vector3& pos);

#pragma region Setter

	/// <summary>
	/// �X�e�[�W�|�C���^�Z�b�g
	/// </summary>
	/// <param name="blocks"></param>
	void SetPStage(Stage* stage) { this->stage = stage; }
#pragma endregion

#pragma region Getter

#pragma endregion
};

