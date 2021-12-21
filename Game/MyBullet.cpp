#include "MyBullet.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "DebugText.h"
#include "GameUtility.h"
#include "Input.h"

#include "BreakFloor.h"
#include "Easing.h"

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

	//�G�t�F�N�g�^�C�}�[
	if (GameUtility::GetNowPhase() == PHASE_FIRSTEFFECT) {
		firstEffectTimer.SetTimer(0, 3500);
		firstEffectTimer.Start();
	}
	//2��ڈȍ~�͋��̉��o�̂�
	else {
		firstEffectTimer.SetTimer(2500, 3500);
		firstEffectTimer.Start();
	}
}

void MyBullet::Update()
{
	if (GameUtility::GetNowPhase() != PHASE_AFTERSHOOT) {
		UpdateFirstEffect();
		UpdateBeforeShoot();
		//���X�V
		objArrow.Update();
	}
	else {
		//�Փ˂��N���邩�`�F�b�N
		CheckCollision();
		ApplyGravity();
		Move();
	}
	//���X�V
	objSphere.SetPosition(position);
	objSphere.Update();
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

void MyBullet::UpdateFirstEffect()
{
	firstEffectTimer.Update();

	double y = Easing::GetEaseValue(EASE_OUTBOUNCE, 200, RADIUS, firstEffectTimer, 2500, 3500);

	position.y = y;
}

void MyBullet::UpdateBeforeShoot()
{
	//�ˏo�O�A�ʒu�����߂�����
	if (GameUtility::GetNowPhase() != PHASE_SETANGLE) {

		DecideShootPos();

		//�N���b�N�Ō���A�p�x�Z�b�g�t�F�[�Y�Ɉڂ�
		if (GameUtility::GetNowPhase() == PHASE_SETPOS &&
			Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {
			//���`�悪����Ȃ��悤�Ɋp�x����
			DecideShootAngle();
			//�t�F�[�Y���ڂ�
			GameUtility::SetNowPhase(PHASE_SETANGLE);
		}
	}
	//�ˏo�O�A�p�x�����߂�����
	else {
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
		blockPos.y = position.y;
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
}

void MyBullet::ApplyGravity()
{
	//���̏�ɂ��Ȃ�������d�͓K�p
	if (IsOutStage(position)) {
		gravity += 0.02f;
		position.y -= gravity;
	}
	else {
		gravity = 0;
		position.y = RADIUS;
	}
}

void MyBullet::CheckCollision()
{
	//���C�X�V
	UpdateRay();

	//����������������Փ˔���𖳎�
	if (position.y < -RADIUS * 3 * 0.5f) { return; }

	CheckBlockCollision();
	CheckFloorCollision();
}

void MyBullet::CheckBlockCollision()
{
	//���C�ƃJ�v�Z���̋����̍ŒZ�Z�o�p
	float alreadyHitDistance = FLT_MAX;

	//���C�ƃu���b�N���Փ˂��邩
	for (int i = 0; i < stage->GetBlocks().size(); i++) {
		bool isCollision = false;

		//���ƃu���b�N�Ƃ̋����Z�o
		Vector3 blockPos = stage->GetBlocks()[i]->GetPosition();
		blockPos.y = position.y;	//xz���ʂŎZ�o������
		float lengthSq = (blockPos - position).LengthSq();

		//�Ώۃu���b�N���牓�������画�肵�Ȃ�
		bool nearFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH * 2;
		if (nearFloor == false) {
			continue;
		}

		//�u���b�N�����e�J�v�Z������ɂ�����
		for (int j = 0; j < stage->GetBlocks()[i]->GetCapsule().size(); j++) {
			float distance = 0;
			Vector3 normal = {};
			//���t���[���ړ�����
			float nextMoveLength = (velocity * speed).Length();

			//��������̃��C�ƃu���b�N�̃J�v�Z�����������Ă��邩
			bool isHitRay2Capsule = Collision::CheckRay2Capsule(ray, stage->GetBlocks()[i]->GetCapsule()[j], &distance, nullptr, nullptr, &normal);
			//���̈ړ��ŏՓ˂��邩
			bool isHitNextMove = distance < nextMoveLength;
			//���ɓ��������J�v�Z���Ƃ̋������Z����
			bool isHitCorrectCapsule = distance < alreadyHitDistance;

			if (isHitRay2Capsule && isHitNextMove && isHitCorrectCapsule) {
				//���ɓ��������J�v�Z���Ƃ̋����X�V
				alreadyHitDistance = distance;

				//NextMoveInfo�ɏ�����������
				nextMoveInfo.isCollisionNextFrame = true;

				//���̈ړ��̃x�N�g���v�Z(���˃x�N�g��)
				nextMoveInfo.nextVel = (velocity - 2.0f * velocity.Dot(normal) * normal);

				//���̈ړ���̈ʒu�v�Z
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
}

void MyBullet::CheckFloorCollision()
{
	//���Ə��M�~�b�N�Ƃ̔���
	for (int i = 0; i < stage->GetFloors().size(); i++) {

		//�m�[�}���u���b�N�͓��ɉ������Ȃ�
		if (stage->GetFloors()[i]->GetObjectType() == "NormalFloor") {
			continue;
		}

		//���Ə��u���b�N�Ƃ̋����Z�o
		Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
		floorPos.y = position.y;	//xz���ʂŎZ�o������
		float lengthSq = (floorPos - position).LengthSq();

		//�Ώۏ��u���b�N���牓�������画�肵�Ȃ�
		bool nearFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH * 3 / 2;
		if (nearFloor == false) {
			continue;
		}

		if (stage->GetFloors()[i]->GetObjectType() == "BreakFloor") {
			BreakFloor* floor = dynamic_cast<BreakFloor*>(stage->GetFloors()[i]);

			//�������u���b�N���痣�ꂽ�������m�����邽�߂Ƀt���O���Z�b�g
			//(���m��BreakFloor��Update()�ōs��)
			bool onFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH;
			floor->SetOnFloor(onFloor);
		}
		else {
			//���S�ɋ߂��ʒu�ɏ������L��
			bool onFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH / 4;
			if (onFloor) {
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
		isHole = true;

	Vector3 floorPos = { 0,0,0 };
	Vector2 floorSize = { stage->GetStageSize().x * ONE_CELL_LENGTH, stage->GetStageSize().y * ONE_CELL_LENGTH };
	isOutside = pos.x < floorPos.x - floorSize.x / 2 ||
		pos.x > floorPos.x + floorSize.x / 2 ||
		pos.z < floorPos.y - floorSize.y / 2 ||
		pos.z > floorPos.y + floorSize.y / 2;

	if (GameUtility::GetNowPhase() != PHASE_AFTERSHOOT) {
		return isOutside;
	}

	//���u���b�N��艺�ɂ���Ƃ��A���͗��������Ă���̂ŏ��Ƃ̔�����X���[
	if (pos.y >= -RADIUS) {
		//���Ə��M�~�b�N�Ƃ̔���
		for (int i = 0; i < stage->GetFloors().size(); i++) {
			//���̋����ȏ�̈ʒu�ɂ�����͔̂��肵�Ȃ�
			Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
			floorPos.y = pos.y;
			float lengthSq = (floorPos - pos).LengthSq();

			//�Ώۏ��u���b�N�ɏ���Ă��Ȃ������画�肵�Ȃ�
			bool nearFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH;
			if (nearFloor == false) {
				continue;
			}

			//����Ă鏰�u���b�N�����ȊO�Ȃ猊�ɗ����Ă��Ȃ��\��
			if (stage->GetFloors()[i]->GetObjectType() != "HoleFloor") {
				if (pos.y >= RADIUS) {
					isHole = false;
				}
				//�X�s�[�h��0.5�ȏォ����蔼���ȏ��̈ʒu�ɂ���Ε��A
				else if (speed >= 0.5f && pos.y > 0) {
					isHole = false;
					//�����Ă����Ԃ��畜�A����Ƃ��͍����ɉ����ăX�s�[�h�𗎂Ƃ�
					speed -= (ONE_CELL_LENGTH / 2 - pos.y) * 0.5f;
					
				}
				else {
					//���u���b�N�̑��ʂƔ��˂�����
					velocity.x *= -1;
					velocity.z *= -1;
				}

				break;
			}
		}
	}

	return isOutside || isHole;

}
