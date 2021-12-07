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
	//モデル生成
	modelSphere.CreateSphere(20, 20, ONE_CELL_LENGTH / 2, true);
	modelArrow.CreateSquareTex(8, "Arrow.png", { 1,1,1 });
}

void MyBullet::Initialize()
{
	//パラメータ初期化
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	position = { 0, ONE_CELL_LENGTH / 2, z };
	velocity = { 0,0,0 };
	speed = 1.0f;
	shotAngle = 90;
	isShoot = false;

	//球オブジェクト
	objSphere.Initialize();
	objSphere.SetObjModel(&modelSphere);
	//矢印オブジェクト
	objArrow.Initialize();
	objArrow.SetObjModel(&modelArrow);

	nextMoveInfo = {};
}

void MyBullet::Update()
{
	if (GameUtility::GetNowPhase() != PHASE_AFTERSHOOT) {
		UpdateBeforeShoot();
		//球更新
		objSphere.SetPosition(position);
		objSphere.Update();
		//矢印更新
		objArrow.Update();
	}
	else {
		Move();
		//球更新
		objSphere.SetPosition(position);
		objSphere.Update();
		//次フレームで衝突が起こるかチェック
		CheckCollision();
	}

}

void MyBullet::Draw()
{
	//3Dオブジェクト描画
	objSphere.Draw();

	//矢印描画
	if (GameUtility::GetNowPhase() == PHASE_SETANGLE) {
		objArrow.Draw();
	}
}

void MyBullet::UpdateBeforeShoot()
{
	//射出前、位置を決めさせる
	if (GameUtility::GetNowPhase() == PHASE_SETPOS) {
		//スタート位置のz座標取得
		float z;
		GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

		//マウスとレイとの交点のx座標取得
		Vector3 mouse;
		Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage->GetFloor().GetPlane(), nullptr, &mouse);

		position = { mouse.x, position.y, z };

		//クリックで決定、角度セットフェーズに移る
		if (Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {
			//矢印描画が崩れないように角度決定
			DecideShootAngle();
			//フェーズを移す
			GameUtility::SetNowPhase(PHASE_SETANGLE);
		}
	}
	//射出前、角度を決めさせる
	else if (GameUtility::GetNowPhase() == PHASE_SETANGLE) {
		//角度決定
		shotAngle = DecideShootAngle();

		//クリックで決定、射出後フェーズに移る
		if (Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {

			//射出
			Shoot();

			//フェーズを移す
			GameUtility::SetNowPhase(PHASE_AFTERSHOOT);
		}
	}
}

float MyBullet::DecideShootAngle()
{
	Vector3 center = position;
	Vector3 mouse = {};

	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage->GetFloor().GetPlane(), nullptr, &mouse);
	//マウスのy座標を地面と平行に
	mouse.y = position.y;

	//射出角度算出
	float angle = atan2(mouse.z - center.z, mouse.x - center.x);

	//矢印の座標と回転更新
	Vector3 vec = mouse - center;
	vec = vec.Normalize();
	Vector3 arrowPos = center + vec * 10;
	objArrow.SetPosition(arrowPos);
	objArrow.SetRotation(90, -angle * 180 / PI, 0);

	return angle;
}

void MyBullet::Shoot()
{
	//移動量計算
	velocity = { cos(shotAngle), 0, sin(shotAngle) };

	//念のため正規化
	velocity = velocity.Normalize();

	isShoot = true;
}

void MyBullet::Move()
{
	//このフレームで衝突するなら、事前に計算された位置などを適用
	if (nextMoveInfo.isCollisionNextFrame) {
		position = nextMoveInfo.nextPos;
		velocity = nextMoveInfo.nextVel;

		nextMoveInfo.isCollisionNextFrame = false;
	}
	else {
		//通常座標更新
		position += velocity * speed;
	}
}

void MyBullet::CheckCollision()
{
	//Todo:	このままだとブロックが増えると当たり判定が厳しいので
	//		どうにかして軽くさせる


	//レイ更新
	UpdateRay();

	//レイとカプセルの距離の最短算出用
	float alreadyHitDistance = FLT_MAX;

	//レイとブロックが衝突するか
	for (int i = 0; i < stage->GetBlocks().size(); i++) {
		//ブロックが持つ各カプセル判定において
		for (int j = 0; j < stage->GetBlocks()[i]->GetCapsule().size(); j++) {
			float distance = 0;
			Vector3 normal = {};
			//次フレーム移動長さ
			float nextMoveLength = (velocity * speed).Length();

			//自球からのレイとブロックのカプセルが当たっているか
			bool isHitRay2Capsule = Collision::CheckRay2Capsule(ray, stage->GetBlocks()[i]->GetCapsule()[j], &distance, nullptr, nullptr, &normal);
			//次のフレームで衝突するか
			bool isHitNextFlame = distance < nextMoveLength;
			//既に当たったカプセルとの距離が短いか
			bool isHitCorrectCapsule = distance < alreadyHitDistance;

			if (isHitRay2Capsule && isHitNextFlame && isHitCorrectCapsule) {
				//既に当たったカプセルとの距離更新
				alreadyHitDistance = distance;

				//NextMoveInfoに情報を書き込む
				nextMoveInfo.isCollisionNextFrame = true;

				//次フレームの移動量計算(反射ベクトル)
				nextMoveInfo.nextVel = (velocity - 2.0f * velocity.Dot(normal) * normal);

				//次フレームの位置計算
				Vector3& newPos = nextMoveInfo.nextPos;
				newPos = position;
				float mul = (distance / nextMoveLength);
				//壁に当たるまで移動
				newPos += velocity * speed * mul;
				//新しい移動量で残りの長さを移動
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