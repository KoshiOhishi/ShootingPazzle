#pragma once
#include "InstancingObject.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"

class BaseFloor
{
protected:
	InstancingObject object;

	//出現エフェクトの種類
	int firstEffectType = -1;
	//出現エフェクトの終了時間
	double firstEffectEndTime = -1;
	//クリアエフェクトの開始時間
	double clearEffectStartTime = -1;

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseFloor() {};

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(const StageVec2& pos) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 出現エフェクトの更新
	/// </summary>
	virtual void UpdateFirstEffect(const Timer& timer);

	/// <summary>
	/// クリアエフェクトの更新
	/// </summary>
	/// <param name="timer"></param>
	virtual void UpdateClearEffect(const Timer& timer);

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	virtual void UpdateCollision() = 0;

	/// <summary>
	/// 座標セット (Y座標固定)
	/// </summary>
	/// <param name="pos">ステージ上の座標</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// ブロックの種類を取得する
	/// </summary>
	/// <returns>ブロックの種類</returns>
	virtual std::string GetObjectType() = 0;

	const Vector3& GetPosition() { return object.GetPosition(); }
};

