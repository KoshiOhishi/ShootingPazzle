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
	void Initialize(float posX, float posZ, float sphereRadius) override;

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

	const std::string& GetObjectType() override { return "SquareBlock"; }
};

