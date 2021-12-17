#pragma once
#include "BaseFloor.h"

enum TurnType
{
	TURNTYPE_LEFT,
	TURNTYPE_RIGHT,
	TURNTYPE_UP,
	TURNTYPE_DOWN
};

class TurnFloor :
    public BaseFloor
{
private:
	static InstancingObjectDraw instancingObjectDraw[4];
	static ObjModel modelBox[4];
	int turnType;

public:
	static void CreateModel();

	static void StaticInitialize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const StageVec2& pos) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw(int index = -1);

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;

	virtual std::string GetObjectType() override { return "TurnFloor_" + std::to_string(turnType); }

	/// <summary>
	/// 方向転換の向き指定
	/// </summary>
	/// <param name="turnType">TURNTYPE_...</param>
	void SetTurnType(int turnType);

	static void ResetIndex(int turnType) { instancingObjectDraw[turnType].ResetIndex(); }
};

