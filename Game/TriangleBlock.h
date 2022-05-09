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
	static DX12Library::ObjModel modelTriangle[4];
	//パーティクル (各色)
	static DX12Library::Particle3D particle[5];

public:
	static void StaticInitialize();
	static DX12Library::ObjModel* GetModel(int index) { return &modelTriangle[index]; }
	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="pos">生成位置</param>
	/// <param name="colorIndex">色インデックス</param>
	static void GenerateParticle(int num, const DX12Library::Vector3& pos, int colorIndex);

	/// <summary>
	/// パーティクル描画
	/// </summary>
	static void DrawParticle();

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
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// ブロックの種類を返す
	/// </summary>
	/// <returns></returns>
	int GetBlockType() override;

	/// <summary>
	/// 壊れるまでのカウントを1減らす 0になったらBreakup()を呼ぶ
	/// </summary>
	void DecrementBreakupCount() override;

	/// <summary>
	/// ブロックを壊す
	/// </summary>
	void Breakup() override;
};