#pragma once
#include "BaseBlock.h"

class SquareBlock :
	public BaseBlock
{
private:
	static InstancingObjectDraw instancingObjectDraw[4];
	//正四面体モデル
	static ObjModel modelBox[4];

public:
	static void CreateModel();

public:
	static void StaticInitialize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const StageVec2& pos, float sphereRadius) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw();

	/// <summary>
	/// 描画終了関数
	/// </summary>
	static void EndDraw();

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

