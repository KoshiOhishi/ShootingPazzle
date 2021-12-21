#pragma once
#include "Object3D.h"
#include "ObjModel.h"
#include "Vector2.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"
#include "Stage.h"

class Stage;

class BaseBlock
{
protected:
	static Stage* pStage;

	//球との衝突判定用カプセル
	std::vector<Capsule> capsule;
	//3Dオブジェクト
	Object3D object;
	//衝突後に壊れるブロックか
	bool isBreakable = false;
	//あと何回当たれば壊れるか　初期値0で破壊不可能ブロックになる
	unsigned short breakupCount = 0;

	//出現エフェクトの種類
	int firstEffectType = -1;
	//出現エフェクトが終わる時間
	double effectEndTime = -1;

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
	/// 出現エフェクト時の更新
	/// </summary>
	virtual void UpdateFirstEffect(const Timer& timer);

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
	static void SetPStage(Stage* pStage) { BaseBlock::pStage = pStage; }

	/// <summary>
	/// 座標セット (Y座標固定)
	/// </summary>
	/// <param name="pos">ステージ上の座標</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// 座標セット（主にエフェクト用）
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos) { object.SetPosition(pos); }

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

