#include "MyBullet.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "DebugText.h"
#include "GameUtility.h"
#include "Input.h"

ObjModel MyBullet::modelSphere;
ObjModel MyBullet::modelArrow;
const float MyBullet::RADIUS = ONE_CELL_LENGTH / 2;

void MyBullet::CreateModel()
{
	//���f������
	modelSphere.CreateSphere(20, 20, RADIUS, true);
	modelArrow.CreateSquareTex(8, "Arrow.png", { 1,1,1 });
}

void MyBullet::Initialize()
{
	//�p�����[�^������
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	position = { 0, RADIUS, z };
	velocity = { 0,0,0 };
	speed = 1.0f;
	shotAngle = 90;
	friction = 0.0005f;
	gravity = 0;
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

		DecideShootPos();

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

void MyBullet::DecideShootPos()
{
	//�X�^�[�g�ʒu��z���W�擾
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	//�}�E�X�ƃ��C�Ƃ̌�_��x���W�擾
	Vector3 mouse;
	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage->GetFloorCollision(), nullptr, &mouse);
	mouse.z = z;

	//�X�e�[�W�̊O�A�܂��̓u���b�N�ƏՓ˂��Ă�����ʒu�X�V���Ȃ�
	if (IsOutStage(mouse)) { return; }
	for (int i = 0; i < stage->GetBlocks().size(); i++) {
		Vector3 blockPos = stage->GetBlocks()[i]->GetPosition();
		Vector3 sub = { RADIUS, RADIUS, RADIUS };
		Vector3 spherePos = { mouse.x, position.y, position.z };
		Sphere sphere{ spherePos, RADIUS };
		AABB aabb{ blockPos - sub, blockPos + sub };

		if (Collision::CheckAABB2Sphere(aabb, sphere)) {
			return;
		}
	}

	position = { mouse.x, position.y, z };
}

float MyBullet::DecideShootAngle()
{
	Vector3 center = position;
	Vector3 mouse = {};

	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage->GetFloorCollision(), nullptr, &mouse);
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

	//���C
	speed -= friction;
	if (speed < 0) {
		speed = 0;
	}

	//���̏�ɂ��Ȃ�������d�͓K�p
	bool isFall = position.y < -RADIUS;
	if (IsOutStage(position) || isFall) {
		gravity += 0.025f;
		position.y -= gravity;
	}
	else {
		gravity = 0;
		position.y = RADIUS;
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
		bool isCollision = false;

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

				isCollision = true;
			}
		}

		//�������Ă���΃u���b�N�̉���܂ł̃J�E���g�����炷
		if (isCollision) {
			stage->GetBlocks()[i]->DecrementBreakupCount();
		}
	}

	//���Ə��M�~�b�N�Ƃ̔���
	for (int i = 0; i < stage->GetFloors().size(); i++) {
		//���̋����ȏ�̈ʒu�ɂ�����͔̂��肵�Ȃ�
		Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
		floorPos.y = position.y;
		float lengthSq = (floorPos - position).LengthSq();
		if (lengthSq > ONE_CELL_LENGTH * ONE_CELL_LENGTH) {
			continue;
		}

		//�m�[�}���u���b�N�͓��ɉ������Ȃ�
		if (stage->GetFloors()[i]->GetObjectType() == "NormalFloor") {
			continue;
		}
		else {
			float length = sqrt(lengthSq);
			//���������a��1/2�ŏ�����Ƃ݂Ă������낤
			if (length < RADIUS) {
				//�����]���u���b�N(��)
				if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_0") {
					if (velocity.x != -1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { -1,0,0 };
					velocity = velocity.Normalize();
				}
				//�����]���u���b�N(�E)
				else if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_1") {
					if (velocity.x != 1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { 1,0,0 };
					velocity = velocity.Normalize();
				}
				//�����]���u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_2") {
					if (velocity.z != 1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { 0,0,1 };
					velocity = velocity.Normalize();
				}
				//�����]���u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_3") {
					if (velocity.z != -1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { 0,0,-1 };
					velocity = velocity.Normalize();
				}
				break;
			}
		}
	}
}

void MyBullet::UpdateRay()
{
	ray.start = position;
	ray.dir = velocity;
}

bool MyBullet::IsOutStage(const Vector3& pos)
{
	bool isOutside = false,
		isHole = false;

	Vector3 floorPos = { 0,0,0 };
	Vector2 floorSize = { stage->GetStageSize().x * ONE_CELL_LENGTH, stage->GetStageSize().y * ONE_CELL_LENGTH };
	isOutside = pos.x < floorPos.x - floorSize.x / 2 ||
		pos.x > floorPos.x + floorSize.x / 2 ||
		pos.z < floorPos.y - floorSize.y / 2 ||
		pos.z > floorPos.y + floorSize.y / 2;

	//���Ə��M�~�b�N�Ƃ̔���
	for (int i = 0; i < stage->GetFloors().size(); i++) {
		//���̋����ȏ�̈ʒu�ɂ�����͔̂��肵�Ȃ�
		Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
		floorPos.y = pos.y;
		float lengthSq = (floorPos - pos).LengthSq();
		if (lengthSq > ONE_CELL_LENGTH * ONE_CELL_LENGTH) {
			continue;
		}
		//��
		if (stage->GetFloors()[i]->GetObjectType() == "HoleFloor") {
			isHole = true;
			break;
		}
	}

	return isOutside || isHole;

}
