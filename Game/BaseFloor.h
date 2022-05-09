#pragma once
#include "InstancingObject.h"
#include "ObjModel.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"
#include "Stage.h"
#include "Particle3D.h"

class Stage;

class BaseFloor
{
protected:
	//インスタンシング描画オブジェクト
	DX12Library::InstancingObject object;
	//インスタンシング描画親オブジェクトポインタ
	DX12Library::InstancingObjectDraw* pIOD;

	//ステージポインタ
	static Stage* pStage;
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
	virtual void UpdateFirstEffect(const DX12Library::Timer& timer);

	/// <summary>
	/// クリアエフェクトの更新
	/// </summary>
	/// <param name="timer"></param>
	virtual void UpdateClearEffect(const DX12Library::Timer& timer);

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	virtual void UpdateCollision() = 0;

	/// <summary>
	/// 床ブロックの種類を返す
	/// </summary>
	/// <returns></returns>
	virtual int GetFloorType() = 0;

	/// <summary>
	/// ステージポインタセット
	/// </summary>
	/// <param name="pStage"></param>
	static void SetPStage(Stage* pStage) { BaseFloor::pStage = pStage; }

	/// <summary>
	/// 座標セット (ステージ座標基準)
	/// </summary>
	/// <param name="pos">ステージ上の座標</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// 座標セット
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const DX12Library::Vector3& pos) { object.SetPosition(pos); }

	/// <summary>
	/// インスタンシング描画親オブジェクトのポインタセット
	/// </summary>
	/// <param name="pIOD"></param>
	void SetPInstancingObjectDraw(DX12Library::InstancingObjectDraw* pIOD) { this->pIOD = pIOD; }

	const DX12Library::Vector3& GetPosition() { return object.GetPosition(); }
};

