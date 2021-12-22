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
		firstEffectTimer.SetTimer(0, 3500);
		firstEffectTimer.Start();
	}
}

void GameCamera::Update()
{
	UpdateFirseEffect();

	Camera::Update();
}

void GameCamera::UpdateFirseEffect()
{
	if (GameUtility::GetNowPhase() != PHASE_FIRSTEFFECT) {
		return;
	}

	firstEffectTimer.Update();
	//�ړ��J�n�����i�^�C�}�[�Ԏw��j
	double start = 1500;
	//�ړ��I�������i�^�C�}�[�Ԏw��j
	double end = 2500;

	//�ړ���
	Vector3 move = afterEffectPos - beforeEffectPos;
	//��]��
	Vector3 rot = afterEffectRot - beforeEffectRot;

	//�ݒ肷��ʒu
	Vector3 nowPos = {};

	//�ݒ肷��p�x
	Vector3 nowRot = {};

	nowPos.x = Easing::GetEaseValue(EASE_INOUTQUAD, beforeEffectPos.x, beforeEffectPos.x + move.x, firstEffectTimer, start, end);
	nowPos.y = Easing::GetEaseValue(EASE_INOUTQUAD, beforeEffectPos.y, beforeEffectPos.y + move.y, firstEffectTimer, start, end);
	nowPos.z = Easing::GetEaseValue(EASE_INOUTQUAD, beforeEffectPos.z, beforeEffectPos.z + move.z, firstEffectTimer, start, end);

	nowRot.x = Easing::GetEaseValue(EASE_INOUTQUAD, beforeEffectRot.x, beforeEffectRot.x + rot.x, firstEffectTimer, start, end);
	nowRot.y = Easing::GetEaseValue(EASE_INOUTQUAD, beforeEffectRot.y, beforeEffectRot.y + rot.y, firstEffectTimer, start, end);
	nowRot.z = Easing::GetEaseValue(EASE_INOUTQUAD, beforeEffectRot.z, beforeEffectRot.z + rot.z, firstEffectTimer, start, end);

	SetPositionAndDistance( nowPos, 15.0f);
	SetRotation(nowRot);
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
	beforeEffectPos = { 0, 50, z - 175 };
	beforeEffectRot = { 0,0,0 };

	//�J�����ړ�����W�Z�b�g
	afterEffectPos = { 0, y, z };
	afterEffectRot = { ANGLE,0,0 };

	return y;
}
