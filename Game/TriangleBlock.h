#pragma once
#include "BaseBlock.h"

enum TriangleType {
	TRIANGLETYPE_NO_LEFTTOP,	//左上の角がない三角形
	TRIANGLETYPE_NO_RIGHTTOP,	//右上の角がない三角形
	TRIANGLETYPE_NO_LEFTBOTTOM,	//左下の角がない三角形
	TRIANGLETYPE_NO_RIGHTBOTTOM,	//右下の角がない三角形
};

class TriangleBlock :
    public BaseBlock
{
private:
	//三角形の向き
	int triangleType;
	//正四面体モデル
	static ObjModel modelTriangle;

public:
	static void CreateModel();

public:

	~TriangleBlock() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const StageVec2& pos, float sphereRadius) override;

	/// <summary>
	/// 三角形の形をセット
	/// </summary>
	/// <param name="syapeType"></param>
	void SetTriangleType(int shapeType);

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

	std::string GetObjectType() override { return "TriangleBlock_" + std::to_string(triangleType); }
};