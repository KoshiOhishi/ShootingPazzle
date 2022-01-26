#pragma once
#include "BaseBlock.h"

class SquareBlock :
	public BaseBlock
{
private:
	//正四面体モデル
	static ObjModel modelBox[4];

public:
	static void StaticInitialize();
	static ObjModel* GetModel(int index) { return &modelBox[index]; }

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const StageVec2& pos, float sphereRadius) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// 壊れるまでのカウントを1減らす 0になったらBreakup()を呼ぶ
	/// </summary>
	void DecrementBreakupCount() override;

	/// <summary>
	/// ブロックを壊す
	/// </summary>
	void Breakup() override;
};

