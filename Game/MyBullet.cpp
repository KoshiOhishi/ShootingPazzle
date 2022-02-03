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
	//モデル生成
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
	//パラメータ初期化
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	//基礎データ初期化
	position = { 0, RADIUS, z };
	velocity = { 0,0,0 };
	speed = 1.5f * FPSManager::GetMulAdjust60FPS();
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
	objArrow.SetDrawShadowToOther(false);

	nextMoveInfo = {};

	changeColorTimer.SetTimer(0, 1000);

	//色は白っぽい色
	myColor[0] = STAGE_COLOR_NONE;
	myColor[1] = STAGE_COLOR_NONE;

	objSphere.SetColor(GameUtility::COLOR_VALUE[STAGE_COLOR_NONE]);

	//効果音ストップ
	GameSound::SetVolume(L"Shooting", 1.0f);
	GameSound::Stop(L"Shooting");
	GameSound::Stop(L"Ascension");
}

void MyBullet::Update()
{
	//ポーズ中は更新無し
	if (GameUtility::GetIsPause() == false) {
		if (GameUtility::GetNowPhase() == PHASE_GAME_FIRSTEFFECT ||
			GameUtility::GetNowPhase() == PHASE_GAME_SETPOS ||
			GameUtility::GetNowPhase() == PHASE_GAME_SETANGLE) {
			//出現エフェクト
			UpdateFirstEffect();
			//発射前更新
			UpdateBeforeShoot();
			//矢印更新
			objArrow.Update();
		}
		else if (GameUtility::GetNowPhase() == PHASE_GAME_AFTERSHOOT) {
			//衝突が起こるかチェック
			CheckCollision();
			//球を動かす
			Move();
			//重力適用
			ApplyGravity();
			//摩擦適用
			ApplyFriction();
			//色更新
			UpdateColor();
		}
		else if (GameUtility::GetNowPhase() == PHASE_GAME_CLEAR) {
			//衝突が起こるかチェック
			CheckCollision();
			//クリアエフェクト
			UpdateClearEffect();
			//球を動かす
			Move();
			//色更新
			UpdateColor();
		}
	}

	//球更新
	objSphere.SetPosition(position);
	objSphere.Update();
}

void MyBullet::Draw()
{
	//出現エフェクト開始以降なら
	//3Dオブジェクト描画
	if (pFirstEffectTimer->GetNowTime() >= MYBULLET_START_FIRST_EFFECT_TIME) {
		objSphere.Draw();
	}

	//矢印描画
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
	//減速→ゆっくり天に昇ってゆく感じ
	if (speed > 0 && velocity.y < 1) {
		speed -= (0.1f * FPSManager::GetMulAdjust60FPS() * FPSManager::GetMulAdjust60FPS());
		if (speed < 0) {
			speed = 0; 
			//効果音鳴らす
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
	//射出前、位置を決めさせる
	if (GameUtility::GetNowPhase() != PHASE_GAME_SETANGLE) {

		DecideShootPos();

		//クリックで決定、角度セットフェーズに移る
		if (GameUtility::GetNowPhase() == PHASE_GAME_SETPOS &&
			Mouse::IsMouseButtonRelease(MouseButton::LEFT)) {
			//効果音鳴らす
			GameSound::Play(L"PosDecide");
			////矢印描画が崩れないように
			//DecideShootAngle();
			//フェーズを移す
			GameUtility::SetNowPhase(PHASE_GAME_SETANGLE);
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
			GameUtility::SetNowPhase(PHASE_GAME_AFTERSHOOT);
		}
	}
}

void MyBullet::DecideShootPos()
{
	//スタート位置のz座標取得
	float z;
	GameUtility::CalcStagePos2WorldPos({ 0,stage->GetStartLaneZ() }, nullptr, &z);

	//矢印オブジェクト更新のために呼び出し
	DecideShootAngle();

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
	arrowPos.y = RADIUS * 2 + 0.05f;
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

	//スピードセット
	speed = 1.5f * FPSManager::GetMulAdjust60FPS();

	//効果音再生
	GameSound::Play(L"Shoot", position);
	GameSound::Play(L"Shooting", position);

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

	//効果音更新
	GameSound::SetPosition(L"Shooting", position);
	GameSound::SetVolume(L"Reflect", speed * FPSManager::GetMulAdjust60FPS());
	GameSound::SetVolume(L"Shooting", speed * FPSManager::GetMulAdjust60FPS());
}

void MyBullet::ApplyFriction()
{
	//摩擦
	speed -= (friction * FPSManager::GetMulAdjust60FPS() * FPSManager::GetMulAdjust60FPS());
	if (speed < 0) {
		speed = 0;
	}
}

void MyBullet::ApplyGravity()
{
	//床の上にいなかったら重力適用
	if (IsOutStage(position)) {
		gravity += (0.1f * FPSManager::GetMulAdjust60FPS() * FPSManager::GetMulAdjust60FPS());
		position.y -= gravity;

		//地面に乗っていないので効果音ストップ
		GameSound::Stop(L"Shooting");
	}
	else {
		gravity = 0;
		position.y = RADIUS;

		//効果音　なっていなければならす
		if (GameSound::IsPlaying(L"Shooting") == false) {
			GameSound::Play(L"Shooting", position);
		}
	}
}

void MyBullet::CheckCollision()
{
	//レイ更新
	UpdateRay(position, velocity);

	//球が落ちきったら衝突判定を無視
	if (position.y < -RADIUS * 3 * 0.5f) { return; }
	//クリアエフェクト時は無視
	if (velocity.y >= 1) { return; }

	CheckBlockCollision();
	CheckFloorCollision();
}

void MyBullet::CheckBlockCollision()
{
	//レイとカプセルの距離の最短算出用
	float alreadyHitDistance = FLT_MAX;
	//動いた距離
	float moveLength = 0;
	//次フレームで球が移動する残りの長さ
	float limitLength = (velocity * speed).Length();

	//計算用
	Vector3 calcPos = position;
	Vector3 calcVel = velocity;

	//レイとブロックが衝突するか判定
	//衝突したらそのブロックとぶつかるまで移動→残りの移動の長さ分で再び判定
	//残りの移動の長さが0になる(次の移動で衝突しなくなる)まで判定
	while (1) {
		int breakIndex = -1;

		//ブロック全て
		for (int i = 0; i < stage->GetBlocks().size(); i++) {

			//球とブロックとの距離算出
			Vector3 blockPos = stage->GetBlocks()[i]->GetPosition();
			blockPos.y = position.y;	//xz平面で算出したい
			float lengthSq = (blockPos - position).LengthSq();

			//対象ブロックから遠かったら判定しない
			bool nearFloor = lengthSq <= ONE_CELL_LENGTH * 1.4142f * ONE_CELL_LENGTH * 1.4142f * 2;
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
				float nextMoveLength = limitLength;

				//自球からのレイとブロックのカプセルが当たっているか
				bool isHitRay2Capsule = Collision::CheckRay2Capsule(ray, stage->GetBlocks()[i]->GetCapsule()[j], &distance, nullptr, nullptr, &normal);
				//次の移動で衝突するか
				bool isHitNextMove = distance < nextMoveLength;
				//既に当たったカプセルとの距離が短いか
				bool isHitCorrectCapsule = distance < alreadyHitDistance;

				if (isHitRay2Capsule && isHitNextMove && isHitCorrectCapsule) {
					//既に当たったカプセルとの距離更新
					alreadyHitDistance = distance;

					//次フレームでブロックと当たる
					nextMoveInfo.isCollisionNextFrame = true;

					//次の移動後のベクトル計算(反射ベクトル)
					nextMoveInfo.nextVel = (calcVel - 2.0f * calcVel.Dot(normal) * normal);

					//次の移動後の位置計算
					float mul = (distance / nextMoveLength);
					nextMoveInfo.nextPos = calcPos;
					//壁に当たるまで移動
					nextMoveInfo.nextPos += calcVel * speed * mul;

					//残りの長さを記憶
					moveLength = limitLength * mul;

					breakIndex = i;
				}
			}
		}

		//ブロックと当たっていれば、壊れるまでのカウントを減らす
		if (breakIndex != -1) {
			stage->GetBlocks()[breakIndex]->DecrementBreakupCount();
			//ブロックと反射するSE再生
			GameSound::Play(L"Reflect", position);
		}
		//衝突しなかったら最後の衝突した位置から残りの長さを移動させて終了
		else {
			nextMoveInfo.nextPos += calcVel * limitLength;
			break;
		}

		//次ループの判定用に情報更新
		//次フレームで球が移動する残りの長さ
		limitLength -= moveLength;
		//計算に使う位置と移動量
		calcPos = nextMoveInfo.nextPos;
		calcVel = nextMoveInfo.nextVel;
		//今回の判定で移動した量
		moveLength = 0;
		alreadyHitDistance = FLT_MAX;
		//レイを最後の衝突した位置と移動量に更新
		UpdateRay(calcPos, calcVel);
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
						//自分の色を変える
						ChangeColor(STAGE_COLOR_NONE);
						//効果音再生
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_NONE);
				}
				//スイッチブロック(赤)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Red") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_RED) {
						//自分の色を変える
						ChangeColor(STAGE_COLOR_RED);
						//効果音再生
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_RED);
				}
				//スイッチブロック(青)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Blue") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_BLUE) {
						//自分の色を変える
						ChangeColor(STAGE_COLOR_BLUE);
						//効果音再生
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_BLUE);
				}
				//スイッチブロック(黄)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Yellow") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_YELLOW) {
						//自分の色を変える
						ChangeColor(STAGE_COLOR_YELLOW);
						//効果音再生
						GameSound::Play(L"ChangeColor", position);
					}
					GameUtility::SetStageColor(STAGE_COLOR_YELLOW);
				}
				//スイッチブロック(緑)
				else if (stage->GetFloors()[i]->GetObjectName() == "SwitchFloor_Green") {
					if (GameUtility::GetStageColor() != STAGE_COLOR_GREEN) {
						//自分の色を変える
						ChangeColor(STAGE_COLOR_GREEN);
						//効果音再生
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

	//射出前、またはステージの外にいたら穴との判定を取らない
	if (GameUtility::GetNowPhase() != PHASE_GAME_AFTERSHOOT || isOutside) {
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
				//床より半分以上上の位置にいる
				else if (pos.y > 0) {
					//落ちている状態から復帰するときは高さに応じてスピードを落とす
					float newSpeed = speed * (pos.y) / (ONE_CELL_LENGTH / 2);

					//スピードが0でなければ復帰
					if (newSpeed > 0) {
						speed = newSpeed;
						isHole = false;
					}
					else {
						//床ブロックの側面と反射させる
						velocity.x *= -1;
						velocity.z *= -1;
					}
					
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

void MyBullet::ChangeColor(int newColor)
{
	//色を入れ替え
	myColor[0] = myColor[1];
	myColor[1] = newColor;

	//タイマースタート
	changeColorTimer.Reset();
	changeColorTimer.Start();
}

void MyBullet::UpdateColor()
{
	changeColorTimer.Update();

	float r = 0, g = 0, b = 0;
	double	start = changeColorTimer.GetStartTime(),
			end = changeColorTimer.GetEndTime(),
			middle = (end - start) / 4;	//ちょっと早い位置で

	//前の色→白
	if (changeColorTimer.GetNowTime() < middle) {
		r = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[myColor[0]].x, 2, changeColorTimer, start, middle);
		g = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[myColor[0]].y, 2, changeColorTimer, start, middle);
		b = Easing::GetEaseValue(EASE_OUTEXPO, GameUtility::COLOR_VALUE[myColor[0]].z, 2, changeColorTimer, start, middle);
	}
	//白→今の色
	else {
		r = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[myColor[1]].x, changeColorTimer, middle, end);
		g = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[myColor[1]].y, changeColorTimer, middle, end);
		b = Easing::GetEaseValue(EASE_OUTEXPO, 2, GameUtility::COLOR_VALUE[myColor[1]].z, changeColorTimer, middle, end);
	}

	//適用
	objSphere.SetColor(r, g, b, 1);
}
