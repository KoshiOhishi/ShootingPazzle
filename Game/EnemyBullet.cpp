#include "EnemyBullet.h"
#include "Collision.h"
#include "SphereCollider.h"

const float EnemyBullet::ENEMYBULLET_OBJ_RADIUS = 2.5f;

EnemyBullet::EnemyBullet()
{
	//���f������
	modelSphere.CreateSphere(20, 20, ENEMYBULLET_OBJ_RADIUS, true);

	//�I�u�W�F�N�g����
	Object3D::Initialize();
	Object3D::SetObjModel(&modelSphere);
	Object3D::SetCollider(new SphereCollider({ 0,0,0 }, ENEMYBULLET_OBJ_RADIUS));
	Object3D::SetColorAs0To255(225, 64, 225);
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Initialize()
{
	//�p�����[�^������
	const float INIT_POS_Z = -45;

	position = { 0, ENEMYBULLET_OBJ_RADIUS, INIT_POS_Z };
	velocity = { 0,0,0 };
	speed = 1.0f;
	shotAngle = 90;
	isShot = false;
}

void EnemyBullet::Update()
{
	//���W�X�V
	position += velocity * speed;

	//3D�I�u�W�F�N�g�̍��W�X�V
	Object3D::SetPosition(position);

	Object3D::Update();
}

void EnemyBullet::OnCollision(const CollisionInfo& info)
{
	const char* name = typeid(info.object).name();
	if (name == "Block") {
	}
	ChangeVelocity(info.object->GetPosition());
}

void EnemyBullet::Draw()
{
	//3D�I�u�W�F�N�g�`��
	Object3D::Draw();
}

void EnemyBullet::Shot()
{
	//�ړ��ʌv�Z
	velocity = { cos(shotAngle), 0, sin(shotAngle) };

	isShot = true;
}

void EnemyBullet::ChangeVelocity(const Vector3& blockPos)
{
	//�߂荞�ݖh�~����
	position += -velocity * speed;

	//�ǂ̖ʂɓ��������̂����f
	Vector3 p1 = blockPos;
	Vector3 p2 = position;

	//�u���b�N����e�ւ̃x�N�g��
	Vector3 vec = p2 - p1;
	vec = vec.Normalize();

	//�p�x���瓖�������ʂ��Z�o
	float vecAngle = atan2(vec.z, vec.x);

	//�e�̍��ɓ��������Ƃ�
	if ((vecAngle >= 0 && vecAngle < PI / 4) || (vecAngle >= PI * -1 / 4 && vecAngle < 0)) {
		velocity.x *= -1;
	}
	//�e�̉��ɓ��������Ƃ�
	else if ((vecAngle >= PI / 4 && vecAngle < PI * 3 / 4)) {
		velocity.z *= -1;
	}
	//�e�̉E�ɓ��������Ƃ�
	else if ((vecAngle >= PI * 3 / 4 && vecAngle < PI) || (vecAngle >= -PI && vecAngle < PI * -3 / 4)) {
		velocity.x *= -1;
	}
	//�e�̏�ɓ��������Ƃ�
	else if ((vecAngle >= PI * -3 / 4 && vecAngle < PI * -1 / 4)) {
		velocity.z *= -1;
	}

	//���W�X�V
	position += velocity * speed;

	//�R���C�_�[�A�b�v�f�[�g
	collider->Update();
}

void EnemyBullet::SetShotAngle(float angle)
{
	shotAngle = angle;
}
