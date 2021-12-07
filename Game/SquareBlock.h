#pragma once
#include "Block.h"

class SquareBlock :
	public Block
{
private:
	//正四面体モデル
	static ObjModel modelBox;

public:
	static void CreateModel();

public:

	~SquareBlock() override;

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
	void Draw() override;

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

	std::string GetObjectType() override { return "SquareBlock"; }
};

