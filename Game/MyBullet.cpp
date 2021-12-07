#include "MyBullet.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "DebugText.h"
#include "GameUtility.h"
#include "Input.h"

ObjModel MyBullet::modelSphere;
ObjModel MyBullet::modelArrow;

void MyBullet::CreateModel()
{
	//���f������
	modelSphere.CreateSphere(20, 20, ONE_CELL_LENGTH / 2, true);
	modelArrow.CreateSquareTex(8, "Arrow.png", { 1,1,1 });
}

void MyBullet::Initialize()
{
	//�p�����[�^������
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	position = { 0, ONE_CELL_LENGTH / 2, z };
	velocity = { 0,0,0 };
	speed = 1.0f;
	shotAngle = 90;
	isShoot = false;

	//���I�u�W�F�N�g
	objSphere.Initialize();
	objSphere.SetObjModel(&modelSphere);
	//���I�u�W�F�N�g
	objArrow.Initialize();
	objArrow.SetObjModel(&modelArrow);

	nextMoveInfo = {};
}

void MyBullet::Update()
{
	if (GameUtility::GetNowPhase() != PHASE_AFTERSHOOT) {
		UpdateBeforeShoot();
		//���X�V
		objSphere.SetPosition(position);
		objSphere.Update();
		//���X�V
		objArrow.Update();
	}
	else {
		Move();
		//���X�V
		objSphere.SetPosition(position);
		objSphere.Update();
		//���t���[���ŏՓ˂��N���邩�`�F�b�N
		CheckCollision();
	}

}

void MyBullet::Draw()
{
	//3D�I�u�W�F�N�g�`��
	objSphere.Draw();

	//���`��
	if (GameUtility::GetNowPhase() == PHASE_SETANGLE) {
		objArrow.Draw();
	}
}

void MyBullet::UpdateBeforeShoot()
{
	//�ˏo�O�A�ʒu�����߂�����
	if (GameUtility::GetNowPhase() == PHASE_SETPOS) {
		//�X�^�[�g�ʒu��z���W�擾
		float z;
		GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

		//�}�E�X�ƃ��C�Ƃ̌�_��x���W�擾
		Vector3 mouse;
		Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage->GetFloor().GetPlane(), nullptr, &mouse);

		position = { mouse.x, position.y, z };

		//�N���b�N�Ō���A�p�x�Z�b�g�t�F�[�Y�Ɉڂ�
		if (Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {
			//���`�悪����Ȃ��悤�Ɋp�x����
			DecideShootAngle();
			//�t�F�[�Y���ڂ�
			GameUtility::SetNowPhase(PHASE_SETANGLE);
		}
	}
	//�ˏo�O�A�p�x�����߂�����
	else if (GameUtility::GetNowPhase() == PHASE_SETANGLE) {
		//�p�x����
		shotAngle = DecideShootAngle();

		//�N���b�N�Ō���A�ˏo��t�F�[�Y�Ɉڂ�
		if (Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {

			//�ˏo
			Shoot();

			//�t�F�[�Y���ڂ�
			GameUtility::SetNowPhase(PHASE_AFTERSHOOT);
		}
	}
}

float MyBullet::DecideShootAngle()
{
	Vector3 center = position;
	Vector3 mouse = {};

	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage->GetFloor().GetPlane(), nullptr, &mouse);
	//�}�E�X��y���W��n�ʂƕ��s��
	mouse.y = position.y;

	//�ˏo�p�x�Z�o
	float angle = atan2(mouse.z - center.z, mouse.x - center.x);

	//���̍��W�Ɖ�]�X�V
	Vector3 vec = mouse - center;
	vec = vec.Normalize();
	Vector3 arrowPos = center + vec * 10;
	objArrow.SetPosition(arrowPos);
	objArrow.SetRotation(90, -angle * 180 / PI, 0);

	return angle;
}

void MyBullet::Shoot()
{
	//�ړ��ʌv�Z
	velocity = { cos(shotAngle), 0, sin(shotAngle) };

	//�O�̂��ߐ��K��
	velocity = velocity.Normalize();

	isShoot = true;
}

void MyBullet::Move()
{
	//���̃t���[���ŏՓ˂���Ȃ�A���O�Ɍv�Z���ꂽ�ʒu�Ȃǂ�K�p
	if (nextMoveInfo.isCollisionNextFrame) {
		position = nextMoveInfo.nextPos;
		velocity = nextMoveInfo.nextVel;

		nextMoveInfo.isCollisionNextFrame = false;
	}
	else {
		//�ʏ���W�X�V
		position += velocity * speed;
	}
}

void MyBullet::CheckCollision()
{
	//Todo:	���̂܂܂��ƃu���b�N��������Ɠ����蔻�肪�������̂�
	//		�ǂ��ɂ����Čy��������


	//���C�X�V
	UpdateRay();

	//���C�ƃJ�v�Z���̋����̍ŒZ�Z�o�p
	float alreadyHitDistance = FLT_MAX;

	//���C�ƃu���b�N���Փ˂��邩
	for (int i = 0; i < stage->GetBlocks().size(); i++) {
		//�u���b�N�����e�J�v�Z������ɂ�����
		for (int j = 0; j < stage->GetBlocks()[i]->GetCapsule().size(); j++) {
			float distance = 0;
			Vector3 normal = {};
			//���t���[���ړ�����
			float nextMoveLength = (velocity * speed).Length();

			//��������̃��C�ƃu���b�N�̃J�v�Z�����������Ă��邩
			bool isHitRay2Capsule = Collision::CheckRay2Capsule(ray, stage->GetBlocks()[i]->GetCapsule()[j], &distance, nullptr, nullptr, &normal);
			//���̃t���[���ŏՓ˂��邩
			bool isHitNextFlame = distance < nextMoveLength;
			//���ɓ��������J�v�Z���Ƃ̋������Z����
			bool isHitCorrectCapsule = distance < alreadyHitDistance;

			if (isHitRay2Capsule && isHitNextFlame && isHitCorrectCapsule) {
				//���ɓ��������J�v�Z���Ƃ̋����X�V
				alreadyHitDistance = distance;

				//NextMoveInfo�ɏ�����������
				nextMoveInfo.isCollisionNextFrame = true;

				//���t���[���̈ړ��ʌv�Z(���˃x�N�g��)
				nextMoveInfo.nextVel = (velocity - 2.0f * velocity.Dot(normal) * normal);

				//���t���[���̈ʒu�v�Z
				Vector3& newPos = nextMoveInfo.nextPos;
				newPos = position;
				float mul = (distance / nextMoveLength);
				//�ǂɓ�����܂ňړ�
				newPos += velocity * speed * mul;
				//�V�����ړ��ʂŎc��̒������ړ�
				newPos += nextMoveInfo.nextVel * speed * (1.0f - mul);
			}
		}
	}
}

void MyBullet::UpdateRay()
{
	ray.start = position;
	ray.dir = velocity;
}