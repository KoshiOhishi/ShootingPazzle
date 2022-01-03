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
	speed = 1.5f;
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

	//エフェクトタイマー
	if (GameUtility::GetNowPhase() == PHASE_FIRSTEFFECT) {
		firstEffectTimer.SetTimer(0, 3500);
		firstEffectTimer.Start();
	}
	//2回目以降は球の演出のみ
	else {
		firstEffectTimer.SetTimer(2500, 3500);
		firstEffectTimer.Start();
	}
}

void MyBullet::Update()
{
	if (GameUtility::GetNowPhase() == PHASE_FIRSTEFFECT || 
		GameUtility::GetNowPhase() == PHASE_SETPOS ||
		GameUtility::GetNowPhase() == PHASE_SETANGLE) {
		//出現エフェクト
		UpdateFirstEffect();
		//発射前更新
		UpdateBeforeShoot();
		//矢印更新
		objArrow.Update();
	}
	else if (GameUtility::GetNowPhase() == PHASE_AFTERSHOOT) {
		//衝突が起こるかチェック
		CheckCollision();
		Move();
		ApplyGravity();
		ApplyFriction();
	}
	else if (GameUtility::GetNowPhase() == PHASE_CLEAR) {
		//衝突が起こるかチェック
		CheckCollision();
		UpdateClearEffect();
		Move();
	}

	//球更新
	objSphere.SetPosition(position);
	objSphere.Update();
}

void MyBullet::Draw()
{
	//出現エフェクト開始以降なら
	//3Dオブジェクト描画
	if (firstEffectTimer.GetNowTime() >= firstEffectTimer.GetEndTime() - 1000) {
		objSphere.Draw();
	}

	//矢印描画
	if (GameUtility::GetNowPhase() == PHASE_SETANGLE) {
		objArrow.Draw();
	}
}

void MyBullet::UpdateFirstEffect()
{
	if (firstEffectTimer.GetIsEnd() == true) {
		return;
	}

	firstEffectTimer.Update();

	double y = Easing::GetEaseValue(EASE_OUTEXPO, bounceInitPosY, RADIUS, firstEffectTimer, firstEffectTimer.GetEndTime() - 1000, firstEffectTimer.GetEndTime());

	position.y = y;
}

void MyBullet::UpdateClearEffect()
{
	//減速→ゆっくり天に昇ってゆく感じ
	if (speed > 0 && velocity.y < 1) {
		speed -= 0.1f;
		if (speed < 0) { speed = 0; }
	}
	else {
		velocity = { 0,1,0 };
		speed += 0.01f;
	}
}

void MyBullet::UpdateBeforeShoot()
{
	//射出前、位置を決めさせる
	if (GameUtility::GetNowPhase() != PHASE_SETANGLE) {

		DecideShootPos();

		//クリックで決定、角度セットフェーズに移る
		if (GameUtility::GetNowPhase() == PHASE_SETPOS &&
			Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {
			//矢印描画が崩れないように角度決定
			DecideShootAngle();
			//フェーズを移す
			GameUtility::SetNowPhase(PHASE_SETANGLE);
		}
	}
	//射出前、角度を決めさせる
	else {
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

void MyBullet::ApplyFriction()
{
	//摩擦
	speed -= friction;
	if (speed < 0) {
		speed = 0;
	}
}

void MyBullet::ApplyGravity()
{
	//床の上にいなかったら重力適用
	if (IsOutStage(position)) {
		gravity += 0.1f;
		position.y -= gravity;
	}
	else {
		gravity = 0;
		position.y = RADIUS;
	}
}

void MyBullet::CheckCollision()
{
	//レイ更新
	UpdateRay();

	//球が落ちきったら衝突判定を無視
	if (position.y < -RADIUS * 3 * 0.5f) { return; }

	CheckBlockCollision();
	CheckFloorCollision();
}

void MyBullet::CheckBlockCollision()
{
	//レイとカプセルの距離の最短算出用
	float alreadyHitDistance = FLT_MAX;

	//レイとブロックが衝突するか
	for (int i = 0; i < stage->GetBlocks().size(); i++) {
		bool isCollision = false;

		//球とブロックとの距離算出
		Vector3 blockPos = stage->GetBlocks()[i]->GetPosition();
		blockPos.y = position.y;	//xz平面で算出したい
		float lengthSq = (blockPos - position).LengthSq();

		//対象ブロックから遠かったら判定しない
		bool nearFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH * 2;
		if (nearFloor == false) {
			continue;
		}

		//ステージの色とブロックの色が同じかつブロックが黒以外なら球はブロックを通過できる
		bool isSameColor = GameUtility::GetStageColor() == stage->GetBlocks()[i]->GetBlockColor();
		if (stage->GetBlocks()[i]->GetBlockColor() != BLOCK_COLOR_NONE && isSameColor) {
			continue;
		}

		//ブロックが持つ各カプセル判定において
		for (int j = 0; j < stage->GetBlocks()[i]->GetCapsule().size(); j++) {
			float distance = 0;
			Vector3 normal = {};
			//次フレーム移動長さ
			float nextMoveLength = (velocity * speed).Length();

			//自球からのレイとブロックのカプセルが当たっているか
			bool isHitRay2Capsule = Collision::CheckRay2Capsule(ray, stage->GetBlocks()[i]->GetCapsule()[j], &distance, nullptr, nullptr, &normal);
			//次の移動で衝突するか
			bool isHitNextMove = distance < nextMoveLength;
			//既に当たったカプセルとの距離が短いか
			bool isHitCorrectCapsule = distance < alreadyHitDistance;

			if (isHitRay2Capsule && isHitNextMove && isHitCorrectCapsule) {
				//既に当たったカプセルとの距離更新
				alreadyHitDistance = distance;

				//NextMoveInfoに情報を書き込む
				nextMoveInfo.isCollisionNextFrame = true;

				//次の移動のベクトル計算(反射ベクトル)
				nextMoveInfo.nextVel = (velocity - 2.0f * velocity.Dot(normal) * normal);

				//次の移動後の位置計算
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
}

void MyBullet::CheckFloorCollision()
{
	//球と床ギミックとの判定
	for (int i = 0; i < stage->GetFloors().size(); i++) {

		//ノーマルブロックは特に何もしない
		if (stage->GetFloors()[i]->GetObjectName() == "NormalFloor") {
			continue;
		}

		//球と床ブロックとの距離算出
		Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
		floorPos.y = position.y;	//xz平面で算出したい
		float lengthSq = (floorPos - position).LengthSq();

		//対象床ブロックから遠かったら判定しない
		bool nearFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH * 2;
		if (nearFloor == false) {
			continue;
		}

		if (stage->GetFloors()[i]->GetObjectName() == "BreakFloor") {
			BreakFloor* floor = dynamic_cast<BreakFloor*>(stage->GetFloors()[i]);

			//球が床ブロックから離れた時を検知させるためにフラグをセット
			//(検知はBreakFloorのUpdate()で行う)
			bool onFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH;
			floor->SetOnFloor(onFloor);
		}
		else {
			//中心に近い位置に乗ったら有効
			bool onFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH / 4;
			if (onFloor) {
				//方向転換ブロック(左)
				if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Left") {
					if (velocity.x != -1) {
						//ここにエフェクト関数
					}
					velocity = { -1,0,0 };
					velocity = velocity.Normalize();
				}
				//方向転換ブロック(右)
				else if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Right") {
					if (velocity.x != 1) {
						//ここにエフェクト関数
					}
					velocity = { 1,0,0 };
					velocity = velocity.Normalize();
				}
				//方向転換ブロック(上)
				else if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Up") {
					if (velocity.z != 1) {
						//ここにエフェクト関数
					}
					velocity = { 0,0,1 };
					velocity = velocity.Normalize();
				}
				//方向転換ブロック(下)
				else if (stage->GetFloors()[i]->GetObjectName() == "TurnFloor_Down") {
					if (velocity.z != -1) {
						//ここにエフェクト関数
					}
					velocity = { 0,0,-1 };
					velocity = velocity.Normalize();
				}

				//スイッチブロック(白)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_None") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_NONE) {
						//ここにエフェクト関数
					}
					GameUtility::SetStageColor(STAGE_COLOR_NONE);
				}
				//スイッチブロック(赤)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Red") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_RED) {
						//ここにエフェクト関数
					}
					GameUtility::SetStageColor(STAGE_COLOR_RED);
				}
				//スイッチブロック(青)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Blue") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_BLUE) {
						//ここにエフェクト関数
					}
					GameUtility::SetStageColor(STAGE_COLOR_BLUE);
				}
				//スイッチブロック(黄)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Yellow") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_YELLOW) {
						//ここにエフェクト関数
					}
					GameUtility::SetStageColor(STAGE_COLOR_YELLOW);
				}
				//スイッチブロック(緑)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Green") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_GREEN) {
						//ここにエフェクト関数
					}
					GameUtility::SetStageColor(STAGE_COLOR_GREEN);
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
	isOutside = 
		pos.x < floorPos.x - floorSize.x / 2 ||
		pos.x > floorPos.x + floorSize.x / 2 ||
		pos.z < floorPos.y - floorSize.y / 2 ||
		pos.z > floorPos.y + floorSize.y / 2;

	//射出前、またはステージの外にいたら穴との判定を取らない
	if (GameUtility::GetNowPhase() != PHASE_AFTERSHOOT || isOutside) {
		return isOutside;
	}

	//床ブロックより下にいるとき、球は落ちきっているので床との判定をスルー
	if (pos.y >= -RADIUS) {
		//球と床ギミックとの判定
		for (int i = 0; i < stage->GetFloors().size(); i++) {
			//一定の距離以上の位置にあるものは判定しない
			Vector3 floorPos = stage->GetFloors()[i]->GetPosition();
			floorPos.y = pos.y;
			float lengthSq = (floorPos - pos).LengthSq();

			//対象床ブロックに乗っていなかったら判定しない
			bool onFloor = lengthSq <= ONE_CELL_LENGTH * ONE_CELL_LENGTH;
			if (onFloor == false) {
				continue;
			}

			//乗ってる床ブロックが穴以外なら穴に落ちていない可能性
			if (stage->GetFloors()[i]->GetObjectName() != "HoleFloor") {
				if (pos.y >= RADIUS) {
					isHole = false;
				}
				//スピードが0.5以上かつ床より半分以上上の位置にいれば復帰
				else if (speed >= 0.5f && pos.y > 0) {
					isHole = false;
					//落ちている状態から復帰するときは高さに応じてスピードを落とす
					speed -= (ONE_CELL_LENGTH / 2 - pos.y) * 0.5f;
					
				}
				else {
					//床ブロックの側面と反射させる
					velocity.x *= -1;
					velocity.z *= -1;
				}

				break;
			}
		}
	}

	return isOutside || isHole;

}
