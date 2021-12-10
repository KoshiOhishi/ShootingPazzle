#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector2.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"

class BaseBlock
{
protected:
	std::vector<Capsule> capsule;
	Object3D object;
	//衝突後に壊れるブロックか
	bool isBreakable = false;
	unsigned short breakupCount = 0;

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseBlock() {};

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(const StageVec2& pos, float sphereRadius) = 0;

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
	virtual std::string GetObjectType() = 0;

	/// <summary>
	/// 壊れるまでのカウントを1減らす 0になったらBreakup()を呼ぶ
	/// </summary>
	virtual void DecrementBreakupCount() = 0;

	/// <summary>
	/// ブロックを壊す
	/// </summary>
	virtual void Breakup() = 0;

#pragma region Setter
	/// <summary>
	/// 座標セット (Y座標固定)
	/// </summary>
	/// <param name="pos">ステージ上の座標</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// 壊れるかどうかのフラグセット
	/// </summary>
	/// <param name="isBreakable"></param>
	void SetBreakupCount(unsigned short breakupCount);

#pragma endregion
#pragma region Getter

	const Vector3& GetPosition() { return object.GetPosition(); }

	/// <summary>
	/// カプセル当たり判定を取得
	/// </summary>
	/// <returns>カプセル当たり判定</returns>
	const std::vector<Capsule> GetCapsule()const {
		return capsule;
	}

	/// <summary>
	/// 壊れるブロックかどうか
	/// </summary>
	/// <returns></returns>
	bool IsBreakable() { return isBreakable; }

	/// <summary>
	/// 壊れるまでの回数を返す
	/// </summary>
	/// <returns></returns>
	unsigned short GetBreakupCount() {return breakupCount;}

	/// <summary>
	/// ブロックが壊れているかどうか
	/// </summary>
	/// <returns></returns>
	bool isBreakup() { return isBreakable && breakupCount <= 0; }

#pragma endregion
};

