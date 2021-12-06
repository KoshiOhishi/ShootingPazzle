#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector2.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"

class Block
{
protected:
	std::vector<Capsule> capsule;
	Object3D object;

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Block() {};

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(float posX, float posZ, float sphereRadius) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	virtual void UpdateCollision() = 0;

	/// <summary>
	/// ブロックの種類を取得する
	/// </summary>
	/// <returns>ブロックの種類</returns>
	virtual const std::string& GetObjectType() = 0;

#pragma region Setter
	/// <summary>
	/// 座標セット (Y座標固定)
	/// </summary>
	/// <param name="pos"></param>
	void SetBlockPos(float x, float z);
#pragma endregion
#pragma region Getter

	const Vector3& GetPosition() { return object.GetPosition(); };

	/// <summary>
	/// カプセル当たり判定を取得
	/// </summary>
	/// <returns>カプセル当たり判定</returns>
	const std::vector<Capsule> GetCapsule()const {
		return capsule;
	}

#pragma endregion
};

