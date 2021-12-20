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

void GameCamera::SetPosFromStageSize(const StageVec2& stageSize)
{
	//�Ƃ肠�����Œ�
	beforeEffectPos = { 0,50,-200 };
	beforeEffectRot = { 0,0,0 };

	afterEffectPos = { 0, 100, -40 };
	afterEffectRot = { 75,0,0 };
}
