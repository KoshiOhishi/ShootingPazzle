#pragma once
#include "InstancingObject.h"
#include "ObjModel.h"
#include "Vector2.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"
#include "Stage.h"
#include "Particle3D.h"

class Stage;

enum BlockColor
{
	BLOCK_COLOR_NONE,
	BLOCK_COLOR_RED,
	BLOCK_COLOR_BLUE,
	BLOCK_COLOR_YELLOW,
	BLOCK_COLOR_GREEN
};

class BaseBlock
{
protected:
	//インスタンシング描画オブジェクト
	InstancingObject object;
	//インスタンシング描画親オブジェクトポインタ
	InstancingObjectDraw* pIOD;
	//ステージポインタ
	static Stage* pStage;
	//球との衝突判定用カプセル
	std::vector<Capsule> capsule;
	//衝突後に壊れるブロックか
	bool isBreakable = false;
	//あと何回当たれば壊れるか　初期値0で破壊不可能ブロックになる
	unsigned short breakupCount = 0;
	//ブロックの色
	int blockColor = BLOCK_COLOR_NONE;
	//前フレームのステージ色
	int prevStageColor = STAGE_COLOR_NONE;
	//色変更タイマー
	Timer changeColorTimer;

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
	/// 出現エフェクト時の更新
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
	/// 壊れるまでのカウントを1減らす 0になったらBreakup()を呼ぶ
	/// </summary>
	virtual void DecrementBreakupCount() = 0;

	/// <summary>
	/// ブロックを壊す
	/// </summary>
	virtual void Breakup() = 0;

	/// <summary>
	/// ブロックの種類を返す
	/// </summary>
	virtual int GetBlockType() = 0;

	/// <summary>
	/// 色更新
	/// </summary>
	void UpdateColor();

#pragma region Setter
	static void SetPStage(Stage* pStage) { BaseBlock::pStage = pStage; }

	/// <summary>
	/// 座標セット (ステージ座標基準)
	/// </summary>
	/// <param name="pos">ステージ上の座標</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// 座標セット
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos) { object.SetPosition(pos); }

	/// <summary>
	/// 壊れるかどうかのフラグセット
	/// </summary>
	/// <param name="isBreakable"></param>
	void SetBreakupCount(unsigned short breakupCount);

	/// <summary>
	/// ブロック色セット
	/// </summary>
	/// <param name="color">BLOCK_COLOR_...</param>
	void SetBlockColor(int blockColor);

	/// <summary>
	/// インスタンシング描画親オブジェクトのポインタセット
	/// </summary>
	/// <param name="pIOD"></param>
	void SetPInstancingObjectDraw(InstancingObjectDraw* pIOD) { this->pIOD = pIOD; }

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
	/// ブロックの色取得
	/// </summary>
	/// <returns></returns>
	int GetBlockColor() { return blockColor; }

	/// <summary>
	/// ブロックが壊れているかどうか
	/// </summary>
	/// <returns></returns>
	bool IsBreakup() { return isBreakable && breakupCount <= 0; }

#pragma endregion
};

