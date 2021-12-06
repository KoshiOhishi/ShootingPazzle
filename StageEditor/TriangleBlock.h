#pragma once
#include "Block.h"

enum ShapeType {
	SHAPETYPE_NO_LEFTTOP,	//左上の角がない三角形
	SHAPETYPE_NO_RIGHTTOP,	//右上の角がない三角形
	SHAPETYPE_NO_LEFTBOTTOM,	//左下の角がない三角形
	SHAPETYPE_NO_RIGHTBOTTOM,	//右下の角がない三角形
};

class TriangleBlock :
    public Block
{
private:
	//三角形の向き
	int shapeType;
	//正四面体モデル
	static ObjModel modelTriangle;

public:
	static void CreateModel();

public:

	~TriangleBlock() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(float posX, float posZ, float sphereRadius) override;

	/// <summary>
	/// 三角形の形をセット
	/// </summary>
	/// <param name="syapeType"></param>
	void SetShapeType(int shapeType);

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

	std::string GetObjectType() override { return "TriangleBlock_" + std::to_string(shapeType); }
};