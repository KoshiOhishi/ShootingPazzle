#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "Block.h"

class EnemyBullet : public Object3D
{
public:
	static const float ENEMYBULLET_OBJ_RADIUS;

private:
	//�ړ���
	Vector3 velocity = {};
	//�ړ����x
	float speed = 0.25f;
	//�ˏo�p�x
	float shotAngle = 0;
	//�����f��
	ObjModel modelSphere;

	//�ˏo�t���O
	bool isShot = false;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	EnemyBullet();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~EnemyBullet();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �����蔻��R�[���o�b�N
	/// </summary>
	void OnCollision(const CollisionInfo& info) override;

	/// <summary>
	/// �e���Z�b�g���ꂽ�p�x�Ŏˏo
	/// </summary>
	void Shot();

	/// <summary>
	/// �u���b�N�̔��˂ňړ��ʂ�ύX����
	/// </summary>
	/// <param name="blockPos">�u���b�N�ʒu</param>
	void ChangeVelocity(const Vector3& blockPos);

#pragma region Setter
	/// <summary>
	/// �ˏo�p�x���Z�b�g
	/// </summary>
	/// <param name="angle">�p�x(radian)</param>
	void SetShotAngle(float angle);

	/// <summary>
	/// �ʒu���Z�b�g (�ˏo�O�p)
	/// </summary>
	/// <param name="pos">�ʒu</param>
	void SetPosition(const Vector3& pos) { position = pos; }

#pragma endregion
#pragma region Getter
	Vector3& GetPosition() { return position; }
#pragma endregion
};

