#include "EnemyBullet.h"
#include "Collision.h"
#include "SphereCollider.h"

const float EnemyBullet::ENEMYBULLET_OBJ_RADIUS = 2.5f;

EnemyBullet::EnemyBullet()
{
	//モデル生成
	modelSphere.CreateSphere(20, 20, ENEMYBULLET_OBJ_RADIUS, true);

	//オブジェクト生成
	Object3D::Initialize();
	Object3D::SetObjModel(&modelSphere);
	Object3D::SetCollider(new SphereCollider({ 0,0,0 }, ENEMYBULLET_OBJ_RADIUS));
	Object3D::SetColorAs0To255(225, 64, 225);
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Initialize()
{
	//パラメータ初期化
	const float INIT_POS_Z = -45;

	position = { 0, ENEMYBULLET_OBJ_RADIUS, INIT_POS_Z };
	velocity = { 0,0,0 };
	speed = 1.0f;
	shotAngle = 90;
	isShot = false;
}

void EnemyBullet::Update()
{
	//座標更新
	position += velocity * speed;

	//3Dオブジェクトの座標更新
	Object3D::SetPosition(position);

	Object3D::Update();
}

void EnemyBullet::OnCollision(const CollisionInfo& info)
{
	const char* name = typeid(info.object).name();
	if (name == "Block") {
	}
	ChangeVelocity(info.object->GetPosition());
}

void EnemyBullet::Draw()
{
	//3Dオブジェクト描画
	Object3D::Draw();
}

void EnemyBullet::Shot()
{
	//移動量計算
	velocity = { cos(shotAngle), 0, sin(shotAngle) };

	isShot = true;
}

void EnemyBullet::ChangeVelocity(const Vector3& blockPos)
{
	//めり込み防止処理
	position += -velocity * speed;

	//どの面に当たったのか判断
	Vector3 p1 = blockPos;
	Vector3 p2 = position;

	//ブロックから弾へのベクトル
	Vector3 vec = p2 - p1;
	vec = vec.Normalize();

	//角度から当たった面を算出
	float vecAngle = atan2(vec.z, vec.x);

	//弾の左に当たったとき
	if ((vecAngle >= 0 && vecAngle < PI / 4) || (vecAngle >= PI * -1 / 4 && vecAngle < 0)) {
		velocity.x *= -1;
	}
	//弾の下に当たったとき
	else if ((vecAngle >= PI / 4 && vecAngle < PI * 3 / 4)) {
		velocity.z *= -1;
	}
	//弾の右に当たったとき
	else if ((vecAngle >= PI * 3 / 4 && vecAngle < PI) || (vecAngle >= -PI && vecAngle < PI * -3 / 4)) {
		velocity.x *= -1;
	}
	//弾の上に当たったとき
	else if ((vecAngle >= PI * -3 / 4 && vecAngle < PI * -1 / 4)) {
		velocity.z *= -1;
	}

	//座標更新
	position += velocity * speed;

	//コライダーアップデート
	collider->Update();
}

void EnemyBullet::SetShotAngle(float angle)
{
	shotAngle = angle;
}
