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
	//モデル生成
	modelSphere.CreateSphere(20, 20, RADIUS, true);
	modelArrow.CreateSquareTex(8, "Arrow.png", { 1,1,1 });
}

void MyBullet::Initialize()
{
	//パラメータ初期化
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	position = { 0, RADIUS, z };
	velocity = { 0,0,0 };
	speed = 1.0f;
	shotAngle = 90;
	friction = 0.0005f;
	gravity = 0;
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

		DecideShootPos();

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

void MyBullet::DecideShootPos()
{
	//スタート位置のz座標取得
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	//マウスとレイとの交点のx座標取得
	Vector3 mouse;
	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage->GetFloorCollision(), nullptr, &mouse);
	mouse.z = z;

	//ステージの外、またはブロックと衝突していたら位置更新しない
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

	//摩擦
	speed -= friction;
	if (speed < 0) {
		speed = 0;
	}

	//床の上にいなかったら重力適用
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
	//Todo:	このままだとブロックが増えると当たり判定が厳しいので
	//		どうにかして軽くさせる

	//レイ更新
	UpdateRay();

	//レイとカプセルの距離の最短算出用
	float alreadyHitDistance = FLT_MAX;

	//レイとブロックが衝突するか
	for (int i = 0; i < stage->GetBlocks().size(); i++) {
		bool isCollision = false;

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

				isCollision = true;
			}
		}

		//当たっていればブロックの壊れるまでのカウントを減らす
		if (isCollision) {
			stage->GetBlocks()[i]->DecrementBreakupCount();
		}
	}

	//球と床ギミックとの判定
	for (int i = 0; i < stage->GetFloors().size(); i++) {
		//一定の距離以上の位置にあるものは判定しない
		Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
		floorPos.y = position.y;
		float lengthSq = (floorPos - position).LengthSq();
		if (lengthSq > ONE_CELL_LENGTH * ONE_CELL_LENGTH) {
			continue;
		}

		//ノーマルブロックは特に何もしない
		if (stage->GetFloors()[i]->GetObjectType() == "NormalFloor") {
			continue;
		}
		else {
			float length = sqrt(lengthSq);
			//距離が半径の1/2で乗ったとみていいだろう
			if (length < RADIUS) {
				//方向転換ブロック(左)
				if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_0") {
					if (velocity.x != -1) {
						//ここにエフェクト関数
					}
					velocity = { -1,0,0 };
					velocity = velocity.Normalize();
				}
				//方向転換ブロック(右)
				else if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_1") {
					if (velocity.x != 1) {
						//ここにエフェクト関数
					}
					velocity = { 1,0,0 };
					velocity = velocity.Normalize();
				}
				//方向転換ブロック(上)
				else if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_2") {
					if (velocity.z != 1) {
						//ここにエフェクト関数
					}
					velocity = { 0,0,1 };
					velocity = velocity.Normalize();
				}
				//方向転換ブロック(下)
				else if (stage->GetFloors()[i]->GetObjectType() == "TurnFloor_3") {
					if (velocity.z != -1) {
						//ここにエフェクト関数
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

	//球と床ギミックとの判定
	for (int i = 0; i < stage->GetFloors().size(); i++) {
		//一定の距離以上の位置にあるものは判定しない
		Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
		floorPos.y = pos.y;
		float lengthSq = (floorPos - pos).LengthSq();
		if (lengthSq > ONE_CELL_LENGTH * ONE_CELL_LENGTH) {
			continue;
		}
		//穴
		if (stage->GetFloors()[i]->GetObjectType() == "HoleFloor") {
			isHole = true;
			break;
		}
	}

	return isOutside || isHole;

}
