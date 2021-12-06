#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "Block.h"

class EnemyBullet : public Object3D
{
public:
	static const float ENEMYBULLET_OBJ_RADIUS;

private:
	//移動量
	Vector3 velocity = {};
	//移動速度
	float speed = 0.25f;
	//射出角度
	float shotAngle = 0;
	//球モデル
	ObjModel modelSphere;

	//射出フラグ
	bool isShot = false;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyBullet();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyBullet();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 当たり判定コールバック
	/// </summary>
	void OnCollision(const CollisionInfo& info) override;

	/// <summary>
	/// 弾をセットされた角度で射出
	/// </summary>
	void Shot();

	/// <summary>
	/// ブロックの反射で移動量を変更する
	/// </summary>
	/// <param name="blockPos">ブロック位置</param>
	void ChangeVelocity(const Vector3& blockPos);

#pragma region Setter
	/// <summary>
	/// 射出角度をセット
	/// </summary>
	/// <param name="angle">角度(radian)</param>
	void SetShotAngle(float angle);

	/// <summary>
	/// 位置をセット (射出前用)
	/// </summary>
	/// <param name="pos">位置</param>
	void SetPosition(const Vector3& pos) { position = pos; }

#pragma endregion
#pragma region Getter
	Vector3& GetPosition() { return position; }
#pragma endregion
};

