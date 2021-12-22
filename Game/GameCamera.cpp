#include "GameCamera.h"
#include "Easing.h"

void GameCamera::Initialize()
{
	Camera::Initialize();
	//camera.SetPositionAndDistance({ 50,50.0f,-40.0f }, 15.0f);
	//camera.SetRotation(60, -60, 0);
	SetPositionAndDistance({ 0,100, -40 }, 15.0f);
	SetRotation(75, 0, 0);

	//エフェクトは最初の読み込みのみ
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
	//移動開始時刻（タイマー間指定）
	double start = 1500;
	//移動終了時刻（タイマー間指定）
	double end = 2500;

	//移動量
	Vector3 move = afterEffectPos - beforeEffectPos;
	//回転量
	Vector3 rot = afterEffectRot - beforeEffectRot;

	//設定する位置
	Vector3 nowPos = {};

	//設定する角度
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

	//奥行で判定をとるので、横幅と縦幅の差が視界の限界(2.5倍)以上であったら縦横の比率を緩和させる
	if (size.x >= size.y * 2.0f) {
		size.y = size.x / 2.0f;
	}

	//視野の範囲にステージがぴったり入っているか判定
	const float SIN_ANGLE = -sin((-ANGLE) * PI / 180);
	const float COS_ANGLE = -cos((-ANGLE) * PI / 180);
	//手前の余裕があればＯＫ
	const float SIN_BACKWARD = sin((-ANGLE - CAMERA_VIEWING_ANGLE / 2) * PI / 180);
	const float COS_BACKWARD = cos((-ANGLE - CAMERA_VIEWING_ANGLE / 2) * PI / 180);

	while (true) {
		//カメラ位置を斜め手前に加算
		y += SIN_ANGLE;
		z += COS_ANGLE;

		//視野の一番下と地面がぶつかる位置算出
		float d1 = Vector3{ 0,1,0 }.Dot({ 0, SIN_BACKWARD, COS_BACKWARD });
		float d2 = Vector3{ 0,1,0 }.Dot({ 0,y,z });
		float t = d2 / -d1;
		float backwardZ = z + t * COS_BACKWARD;

		//2マス分余裕をとる
		float adjust = ONE_CELL_LENGTH * 2;

		//視野の下に2マス分以上の余裕があればループを抜ける
		if (backwardZ <= -size.y / 2 * ONE_CELL_LENGTH - adjust) {
			break;
		}
	}
	
	//カメラ移動前座標セット
	beforeEffectPos = { 0, 50, z - 175 };
	beforeEffectRot = { 0,0,0 };

	//カメラ移動後座標セット
	afterEffectPos = { 0, y, z };
	afterEffectRot = { ANGLE,0,0 };

	return y;
}
