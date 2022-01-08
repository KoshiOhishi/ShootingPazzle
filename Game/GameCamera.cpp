#include "GameCamera.h"
#include "Easing.h"

void GameCamera::Initialize()
{
	Camera::Initialize();
	//camera.SetPositionAndDistance({ 50,50.0f,-40.0f }, 15.0f);
	//camera.SetRotation(60, -60, 0);
	SetPositionAndDistance({ 0,100, -40 }, 15.0f);
	SetRotation(75, 0, 0);

	//�G�t�F�N�g�͍ŏ��̓ǂݍ��݂̂�
	//if (GameUtility::GetNowPhase() == PHASE_FIRSTEFFECT)
	{
		effectTimer.SetTimer(0, 3500);
		effectTimer.Start();
	}
}

void GameCamera::Update()
{
	if (GameUtility::GetNowPhase() == PHASE_GAME_FIRSTEFFECT) {
		UpdateFirstEffect();
	}
	else if (GameUtility::GetNowPhase() == PHASE_GAME_CLEAR) {
		UpdateClearEffect();
	}

	Camera::Update();
}

void GameCamera::UpdateFirstEffect()
{
	effectTimer.Update();

	//�ړ��J�n�����i�^�C�}�[�Ԏw��j
	double start = 1500;
	//�ړ��I�������i�^�C�}�[�Ԏw��j
	double end = 2500;

	//�ړ���
	Vector3 move = afterFirstEffectPos - beforeFirstEffectPos;
	//��]��
	Vector3 rot = afterFirstEffectRot - beforeFirstEffectRot;

	//�ݒ肷��ʒu
	Vector3 setPos = {};

	//�ݒ肷��p�x
	Vector3 setRot = {};

	setPos.x = Easing::GetEaseValue(EASE_INOUTQUAD, beforeFirstEffectPos.x, beforeFirstEffectPos.x + move.x, effectTimer, start, end);
	setPos.y = Easing::GetEaseValue(EASE_INOUTQUAD, beforeFirstEffectPos.y, beforeFirstEffectPos.y + move.y, effectTimer, start, end);
	setPos.z = Easing::GetEaseValue(EASE_INOUTQUAD, beforeFirstEffectPos.z, beforeFirstEffectPos.z + move.z, effectTimer, start, end);

	setRot.x = Easing::GetEaseValue(EASE_INOUTQUAD, beforeFirstEffectRot.x, beforeFirstEffectRot.x + rot.x, effectTimer, start, end);
	setRot.y = Easing::GetEaseValue(EASE_INOUTQUAD, beforeFirstEffectRot.y, beforeFirstEffectRot.y + rot.y, effectTimer, start, end);
	setRot.z = Easing::GetEaseValue(EASE_INOUTQUAD, beforeFirstEffectRot.z, beforeFirstEffectRot.z + rot.z, effectTimer, start, end);

	SetPositionAndDistance( setPos, 15.0f);
	SetRotation(setRot);
}

void GameCamera::UpdateClearEffect()
{
	effectTimer.Update();

	//�ړ��J�n�����i�^�C�}�[�Ԏw��j
	double start = 500;
	//�ړ��I�������i�^�C�}�[�Ԏw��j
	double end = 4800;

	//�ڕW�ʒu
	Vector3 targetPos = afterFirstEffectPos - Vector3{0, 0, 200};

	//�ړ���
	Vector3 move = targetPos - afterFirstEffectPos;
	//��]��
	Vector3 rot = -afterFirstEffectRot - Vector3{ 60,0,0 };

	//�ݒ肷��ʒu
	Vector3 setPos = {};

	//�ݒ肷��p�x
	Vector3 setRot = {};

	setPos.x = Easing::GetEaseValue(EASE_INOUTQUAD, afterFirstEffectPos.x, afterFirstEffectPos.x + move.x, effectTimer, start, end);
	setPos.y = Easing::GetEaseValue(EASE_INOUTQUAD, afterFirstEffectPos.y, afterFirstEffectPos.y + move.y, effectTimer, start, end);
	setPos.z = Easing::GetEaseValue(EASE_INOUTQUAD, afterFirstEffectPos.z, afterFirstEffectPos.z + move.z, effectTimer, start, end);

	setRot.x = Easing::GetEaseValue(EASE_INOUTQUAD, afterFirstEffectRot.x, afterFirstEffectRot.x + rot.x, effectTimer, start, end);
	setRot.y = Easing::GetEaseValue(EASE_INOUTQUAD, afterFirstEffectRot.y, afterFirstEffectRot.y + rot.y, effectTimer, start, end);
	setRot.z = Easing::GetEaseValue(EASE_INOUTQUAD, afterFirstEffectRot.z, afterFirstEffectRot.z + rot.z, effectTimer, start, end);

	SetPositionAndDistance(setPos, 30.0f);
	SetRotation(setRot);
}

void GameCamera::SetCameraParamAfterShoot()
{
	SetPositionAndDistance(afterFirstEffectPos, 30.0f);
	SetRotation(afterFirstEffectRot);
}

float GameCamera::SetPosFromStageSize(const StageVec2& stageSize)
{
	const float ANGLE = 75;
	float y = 0, z = 0;


	StageVec2 size = stageSize;

	//���s�Ŕ�����Ƃ�̂ŁA�����Əc���̍������E�̌��E(2.5�{)�ȏ�ł�������c���̔䗦���ɘa������
	if (size.x >= size.y * 2.0f) {
		size.y = size.x / 2.0f;
	}

	//����͈̔͂ɃX�e�[�W���҂���������Ă��邩����
	const float SIN_ANGLE = -sin((-ANGLE) * PI / 180);
	const float COS_ANGLE = -cos((-ANGLE) * PI / 180);
	//��O�̗]�T������΂n�j
	const float SIN_BACKWARD = sin((-ANGLE - CAMERA_VIEWING_ANGLE / 2) * PI / 180);
	const float COS_BACKWARD = cos((-ANGLE - CAMERA_VIEWING_ANGLE / 2) * PI / 180);

	while (true) {
		//�J�����ʒu���΂ߎ�O�ɉ��Z
		y += SIN_ANGLE;
		z += COS_ANGLE;

		//����̈�ԉ��ƒn�ʂ��Ԃ���ʒu�Z�o
		float d1 = Vector3{ 0,1,0 }.Dot({ 0, SIN_BACKWARD, COS_BACKWARD });
		float d2 = Vector3{ 0,1,0 }.Dot({ 0,y,z });
		float t = d2 / -d1;
		float backwardZ = z + t * COS_BACKWARD;

		//2�}�X���]�T���Ƃ�
		float adjust = ONE_CELL_LENGTH * 2;

		//����̉���2�}�X���ȏ�̗]�T������΃��[�v�𔲂���
		if (backwardZ <= -size.y / 2 * ONE_CELL_LENGTH - adjust) {
			break;
		}
	}
	
	//�J�����ړ��O���W�Z�b�g
	beforeFirstEffectPos = { 0, 50, z - 175 };
	beforeFirstEffectRot = { 0,0,0 };

	//�J�����ړ�����W�Z�b�g
	afterFirstEffectPos = { 0, y, z };
	afterFirstEffectRot = { ANGLE,0,0 };

	return y;
}

void GameCamera::StartEffectTimer(int start, int end, float speed)
{
	effectTimer.SetTimer(start, end, false, speed);
	effectTimer.Start();
}
