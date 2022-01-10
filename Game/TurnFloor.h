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
	static ObjModel modelBox[4];
	int turnType;

public:
	static void CreateModel();
	static ObjModel* GetModel(int index) { return &modelBox[index]; }

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const StageVec2& pos) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// 方向転換の向き指定
	/// </summary>
	/// <param name="turnType">TURNTYPE_...</param>
	void SetTurnType(int turnType);
};

