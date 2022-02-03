#include "MyBullet.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "DebugText.h"
#include "GameUtility.h"
#include "Input.h"
#include "GameSound.h"
#include "FPSManager.h"
#include "GameSound.h"

#include "BreakFloor.h"
#include "Easing.h"

ObjModel MyBullet::modelSphere;
ObjModel MyBullet::modelArrow;
const float MyBullet::RADIUS = ONE_CELL_LENGTH / 2;

void MyBullet::StaticInitialize()
{
	//���f������
	modelSphere.CreateSphere(20, 20, RADIUS, true, { 0.5,0.5,0.5 }, { 0.5,0.5,0.5 }, { 0.25,0.25,0.25 });
	modelArrow.CreatePlaneTex(16, 8, "Resources/Arrow.png", { 1,1,1 });
}

MyBullet::MyBullet()
{
}

MyBullet::~MyBullet()
{
}

void MyBullet::Initialize()
{
	//�p�����[�^������
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	//��b�f�[�^������
	position = { 0, RADIUS, z };
	velocity = { 0,0,0 };
	speed = 1.5f * FPSManager::GetMulAdjust60FPS();
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
	objArrow.SetDrawShadowToOther(false);

	nextMoveInfo = {};

	changeColorTimer.SetTimer(0, 1000);

	//�F�͔����ۂ��F
	myColor[0] = STAGE_COLOR_NONE;
	myColor[1] = STAGE_COLOR_NONE;

	objSphere.SetColor(GameUtility::COLOR_VALUE[STAGE_COLOR_NONE]);

	//���ʉ��X�g�b�v
	GameSound::SetVolume(L"Shooting", 1.0f);
	GameSound::Stop(L"Shooting");
	GameSound::Stop(L"Ascension");
}

void MyBullet::Update()
{
	//�|�[�Y���͍X�V����
	if (GameUtility::GetIsPause() == false) {
		if (GameUtility::GetNowPhase() == PHASE_GAME_FIRSTEFFECT ||
			GameUtility::GetNowPhase() == PHASE_GAME_SETPOS ||
			GameUtility::GetNowPhase() == PHASE_GAME_SETANGLE) {
			//�o���G�t�F�N�g
			UpdateFirstEffect();
			//���ˑO�X�V
			UpdateBeforeShoot();
			//���X�V
			objArrow.Update();
		}
		else if (GameUtility::GetNowPhase() == PHASE_GAME_AFTERSHOOT) {
			//�Փ˂��N���邩�`�F�b�N
			CheckCollision();
			//���𓮂���
			Move();
			//�d�͓K�p
			ApplyGravity();
			//���C�K�p
			ApplyFriction();
			//�F�X�V
			UpdateColor();
		}
		else if (GameUtility::GetNowPhase() == PHASE_GAME_CLEAR) {
			//�Փ˂��N���邩�`�F�b�N
			CheckCollision();
			//�N���A�G�t�F�N�g
			UpdateClearEffect();
			//���𓮂���
			Move();
			//�F�X�V
			UpdateColor();
		}
	}

	//���X�V
	objSphere.SetPosition(position);
	objSphere.Update();
}

void MyBullet::Draw()
{
	//�o���G�t�F�N�g�J�n�ȍ~�Ȃ�
	//3D�I�u�W�F�N�g�`��
	if (pFirstEffectTimer->GetNowTime() >= MYBULLET_START_FIRST_EFFECT_TIME) {
		objSphere.Draw();
	}

	//���`��
	if (GameUtility::GetNowPhase() == PHASE_GAME_SETANGLE) {
		objArrow.Draw();
	}
}

void MyBullet::UpdateFirstEffect()
{
	double y = Easing::GetEaseValue(EASE_OUTEXPO, bounceInitPosY, RADIUS, *pFirstEffectTimer, MYBULLET_START_FIRST_EFFECT_TIME, MYBULLET_END_FIRST_EFFECT_TIME);

	position.y = y;
}

void MyBullet::UpdateClearEffect()
{
	//�������������V�ɏ����Ă䂭����
	if (speed > 0 && velocity.y < 1) {
		speed -= (0.1f * FPSManager::GetMulAdjust60FPS() * FPSManager::GetMulAdjust60FPS());
		if (speed < 0) {
			speed = 0; 
			//���ʉ��炷
			GameSound::Play(L"Ascension");
		}
	}
	else {
		velocity = { 0,1,0 };
		speed += (0.01f * FPSManager::GetMulAdjust60FPS() * FPSManager::GetMulAdjust60FPS());
	}
}

void MyBullet::UpdateBeforeShoot()
{
	//�ˏo�O�A�ʒu�����߂�����
	if (GameUtility::GetNowPhase() != PHASE_GAME_SETANGLE) {

		DecideShootPos();

		//�N���b�N�Ō���A�p�x�Z�b�g�t�F�[�Y�Ɉڂ�
		if (GameUtility::GetNowPhase() == PHASE_GAME_SETPOS &&
			Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {
			//���ʉ��炷
			GameSound::Play(L"PosDecide");
			////���`�悪����Ȃ��悤��
			//DecideShootAngle();
			//�t�F�[�Y���ڂ�
			GameUtility::SetNowPhase(PHASE_GAME_SETANGLE);
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
			GameUtility::SetNowPhase(PHASE_GAME_AFTERSHOOT);
		}
	}
}

void MyBullet::DecideShootPos()
{
	//�X�^�[�g�ʒu��z���W�擾
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	//���I�u�W�F�N�g�X�V�̂��߂ɌĂяo��
	DecideShootAngle();

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
	arrowPos.y = RADIUS * 2 + 0.05f;
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

	//�X�s�[�h�Z�b�g
	speed = 1.5f * FPSManager::GetMulAdjust60FPS();

	//���ʉ��Đ�
	GameSound::Play(L"Shoot", position);
	GameSound::Play(L"Shooting", position);

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

	//���ʉ��X�V
	GameSound::SetPosition(L"Shooting", position);
	GameSound::SetVolume(L"Reflect", speed * FPSManager::GetMulAdjust60FPS());
	GameSound::SetVolume(L"Shooting", speed * FPSManager::GetMulAdjust60FPS());
}

void MyBullet::ApplyFriction()
{
	//���C
	speed -= (friction * FPSManager::GetMulAdjust60FPS() * FPSManager::GetMulAdjust60FPS());
	if (speed < 0) {
		speed = 0;
	}
}

void MyBullet::ApplyGravity()
{
	//���̏�ɂ��Ȃ�������d�͓K�p
	if (IsOutStage(position)) {
		gravity += (0.1f * FPSManager::GetMulAdjust60FPS() * FPSManager::GetMulAdjust60FPS());
		position.y -= gravity;

		//�n�ʂɏ���Ă��Ȃ��̂Ō��ʉ��X�g�b�v
		GameSound::Stop(L"Shooting");
	}
	else {
		gravity = 0;
		position.y = RADIUS;

		//���ʉ��@�Ȃ��Ă��Ȃ���΂Ȃ炷
		if (GameSound::IsPlaying(L"Shooting") == false) {
			GameSound::Play(L"Shooting", position);
		}
	}
}

void MyBullet::CheckCollision()
{
	//���C�X�V
	UpdateRay(position, velocity);

	//����������������Փ˔���𖳎�
	if (position.y < -RADIUS * 3 * 0.5f) { return; }
	//�N���A�G�t�F�N�g���͖���
	if (velocity.y >= 1) { return; }

	CheckBlockCollision();
	CheckFloorCollision();
}

void MyBullet::CheckBlockCollision()
{
	//���C�ƃJ�v�Z���̋����̍ŒZ�Z�o�p
	float alreadyHitDistance = FLT_MAX;
	//����������
	float moveLength = 0;
	//���t���[���ŋ����ړ�����c��̒���
	float limitLength = (velocity * speed).Length();

	//�v�Z�p
	Vector3 calcPos = position;
	Vector3 calcVel = velocity;

	//���C�ƃu���b�N���Փ˂��邩����
	//�Փ˂����炻�̃u���b�N�ƂԂ���܂ňړ����c��̈ړ��̒������ōĂє���
	//�c��̈ړ��̒�����0�ɂȂ�(���̈ړ��ŏՓ˂��Ȃ��Ȃ�)�܂Ŕ���
	while (1) {
		int breakIndex = -1;

		//�u���b�N�S��
		for (int i = 0; i < stage->GetBlocks().size(); i++) {

			//���ƃu���b�N�Ƃ̋����Z�o
			Vector3 blockPos = stage->GetBlocks()[i]->GetPosition();
			blockPos.y = position.y;	//xz���ʂŎZ�o������
			float lengthSq = (blockPos - position).LengthSq();

			//�Ώۃu���b�N���牓�������画�肵�Ȃ�
			bool nearFloor = lengthSq <= ONE_CELL_LENGTH * 1.4142f * ONE_CELL_LENGTH * 1.4142f * 2;
			if (nearFloor == false) {
				continue;
			}

			//�X�e�[�W�̐F�ƃu���b�N�̐F���������u���b�N�����ȊO�Ȃ狅�̓u���b�N��ʉ߂ł���
			bool isSameColor = GameUtility::GetStageColor() == stage->GetBlocks()[i]->GetBlockColor();
			if (stage->GetBlocks()[i]->GetBlockColor() != BLOCK_COLOR_NONE && isSameColor) {
				continue;
			}

			//�u���b�N�����e�J�v�Z������ɂ�����
			for (int j = 0; j < stage->GetBlocks()[i]->GetCapsule().size(); j++) {
				float distance = 0;
				Vector3 normal = {};
				//���t���[���ړ�����
				float nextMoveLength = limitLength;

				//��������̃��C�ƃu���b�N�̃J�v�Z�����������Ă��邩
				bool isHitRay2Capsule = Collision::CheckRay2Capsule(ray, stage->GetBlocks()[i]->GetCapsule()[j], &distance, nullptr, nullptr, &normal);
				//���̈ړ��ŏՓ˂��邩
				bool isHitNextMove = distance < nextMoveLength;
				//���ɓ��������J�v�Z���Ƃ̋������Z����
				bool isHitCorrectCapsule = distance < alreadyHitDistance;

				if (isHitRay2Capsule && isHitNextMove && isHitCorrectCapsule) {
					//���ɓ��������J�v�Z���Ƃ̋����X�V
					alreadyHitDistance = distance;

					//���t���[���Ńu���b�N�Ɠ�����
					nextMoveInfo.isCollisionNextFrame = true;

					//���̈ړ���̃x�N�g���v�Z(���˃x�N�g��)
					nextMoveInfo.nextVel = (calcVel - 2.0f * calcVel.Dot(normal) * normal);

					//���̈ړ���̈ʒu�v�Z
					float mul = (distance / nextMoveLength);
					nextMoveInfo.nextPos = calcPos;
					//�ǂɓ�����܂ňړ�
					nextMoveInfo.nextPos += calcVel * speed * mul;

					//�c��̒������L��
					moveLength = limitLength * mul;

					breakIndex = i;
				}
			}
		}

		//�u���b�N�Ɠ������Ă���΁A����܂ł̃J�E���g�����炷
		if (breakIndex != -1) {
			stage->GetBlocks()[breakIndex]->DecrementBreakupCount();
			//�u���b�N�Ɣ��˂���SE�Đ�
			GameSound::Play(L"Reflect", position);
		}
		//�Փ˂��Ȃ�������Ō�̏Փ˂����ʒu����c��̒������ړ������ďI��
		else {
			nextMoveInfo.nextPos += calcVel * limitLength;
			break;
		}

		//�����[�v�̔���p�ɏ��X�V
		//���t���[���ŋ����ړ�����c��̒���
		limitLength -= moveLength;
		//�v�Z�Ɏg���ʒu�ƈړ���
		calcPos = nextMoveInfo.nextPos;
		calcVel = nextMoveInfo.nextVel;
		//����̔���ňړ�������
		moveLength = 0;
		alreadyHitDistance = FLT_MAX;
		//���C���Ō�̏Փ˂����ʒu�ƈړ��ʂɍX�V
		UpdateRay(calcPos, calcVel);
	}
}

void MyBullet::CheckFloorCollision()
{
	//���Ə��M�~�b�N�Ƃ̔���
	for (int i = 0; i < stage->GetFloors().size(); i++) {

		//�m�[�}���u���b�N�͓��ɉ������Ȃ�
		if (stage->GetFloors()[i]->GetObjectName() == "NormalFloor") {
			continue;
		}

		//���Ə��u���b�N�Ƃ̋����Z�o
		Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
		floorPos.y = position.y;	//xz���ʂŎZ�o������
		float lengthSq = (floorPos - position).LengthSq();

		//�Ώۏ��u���b�N���牓�������画�肵�Ȃ�
		bool nearFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH * 2;
		if (nearFloor == false) {
			continue;
		}

		if (stage->GetFloors()[i]->GetObjectName() == "BreakFloor") {
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
				if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Left") {
					if (velocity.x != -1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { -1,0,0 };
					velocity = velocity.Normalize();
				}
				//�����]���u���b�N(�E)
				else if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Right") {
					if (velocity.x != 1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { 1,0,0 };
					velocity = velocity.Normalize();
				}
				//�����]���u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Up") {
					if (velocity.z != 1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { 0,0,1 };
					velocity = velocity.Normalize();
				}
				//�����]���u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Down") {
					if (velocity.z != -1) {
						//�����ɃG�t�F�N�g�֐�
					}
					velocity = { 0,0,-1 };
					velocity = velocity.Normalize();
				}

				//�X�C�b�`�u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_None") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_NONE) {
						//�����̐F��ς���
						ChangeColor(STAGE_COLOR_NONE);
						//���ʉ��Đ�
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_NONE);
				}
				//�X�C�b�`�u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Red") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_RED) {
						//�����̐F��ς���
						ChangeColor(STAGE_COLOR_RED);
						//���ʉ��Đ�
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_RED);
				}
				//�X�C�b�`�u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Blue") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_BLUE) {
						//�����̐F��ς���
						ChangeColor(STAGE_COLOR_BLUE);
						//���ʉ��Đ�
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_BLUE);
				}
				//�X�C�b�`�u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Yellow") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_YELLOW) {
						//�����̐F��ς���
						ChangeColor(STAGE_COLOR_YELLOW);
						//���ʉ��Đ�
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_YELLOW);
				}
				//�X�C�b�`�u���b�N(��)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Green") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_GREEN) {
						//�����̐F��ς���
						ChangeColor(STAGE_COLOR_GREEN);
						//���ʉ��Đ�
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_GREEN);
				}
				break;
			}
		}
	}
}

void MyBullet::UpdateRay(const Vector3& pos, const Vector3& dir)
{
	ray.start = pos;
	ray.dir = dir;
}

bool MyBullet::IsOutStage(const Vector3& pos)
{
	bool isOutside = false,
		isHole = true;

	Vector3 floorPos = { 0,0,0 };
	Vector2 floorSize = { stage->GetStageSize().x * ONE_CELL_LENGTH, stage->GetStageSize().y * ONE_CELL_LENGTH };
	isOutside = 
		pos.x < floorPos.x - floorSize.x / 2 ||
		pos.x > floorPos.x + floorSize.x / 2 ||
		pos.z < floorPos.y - floorSize.y / 2 ||
		pos.z > floorPos.y + floorSize.y / 2;

	//�ˏo�O�A�܂��̓X�e�[�W�̊O�ɂ����猊�Ƃ̔�������Ȃ�
	if (GameUtility::GetNowPhase() != PHASE_GAME_AFTERSHOOT || isOutside) {
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
			bool onFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH;
			if (onFloor == false) {
				continue;
			}

			//����Ă鏰�u���b�N�����ȊO�Ȃ猊�ɗ����Ă��Ȃ��\��
			if (stage->GetFloors()[i]->GetObjectName() != "HoleFloor") {
				if (pos.y >= RADIUS) {
					isHole = false;
				}
				//����蔼���ȏ��̈ʒu�ɂ���
				else if (pos.y > 0) {
					//�����Ă����Ԃ��畜�A����Ƃ��͍����ɉ����ăX�s�[�h�𗎂Ƃ�
					float newSpeed = speed * (pos.y) / (ONE_CELL_LENGTH / 2);

					//�X�s�[�h��0�łȂ���Ε��A
					if (newSpeed > 0) {
						speed = newSpeed;
						isHole = false;
					}
					else {
						//���u���b�N�̑��ʂƔ��˂�����
						velocity.x *= -1;
						velocity.z *= -1;
					}
					
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

void MyBullet::ChangeColor(int newColor)
{
	//�F�����ւ�
	myColor[0] = myColor[1];
	myColor[1] = newColor;

	//�^�C�}�[�X�^�[�g
	changeColorTimer.Reset();
	changeColorTimer.Start();
}

void MyBullet::UpdateColor()
{
	changeColorTimer.Update();

	float r = 0, g = 0, b = 0;
	double	start = changeColorTimer.GetStartTime(),
			end = changeColorTimer.GetEndTime(),
			middle = (end - start) / 4;	//������Ƒ����ʒu��

	//�O�̐F����
	if (changeColorTimer.GetNowTime() < middle) {
		r = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[myColor[0]].x, 2, changeColorTimer, start, middle);
		g = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[myColor[0]].y, 2, changeColorTimer, start, middle);
		b = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[myColor[0]].z, 2, changeColorTimer, start, middle);
	}
	//�������̐F
	else {
		r = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[myColor[1]].x, changeColorTimer, middle, end);
		g = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[myColor[1]].y, changeColorTimer, middle, end);
		b = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[myColor[1]].z, changeColorTimer, middle, end);
	}

	//�K�p
	objSphere.SetColor(r, g, b, 1);
}
