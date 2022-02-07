#pragma once
#include "BaseBlock.h"

class SquareBlock :
	public BaseBlock
{
private:
	//正四面体モデル
	static ObjModel modelBox[4];
	//パーティクル(各色)
	static Particle3D particle[5];

public:
	static void StaticInitialize();
	static ObjModel* GetModel(int index) { return &modelBox[index]; }

	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="pos">生成位置</param>
	/// <param name="colorIndex">色インデックス</param>
	static void GenerateParticle(int num, const Vector3& pos, int colorIndex);

	/// <summary>
	/// パーティクル描画
	/// </summary>
	static void DrawParticle();

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

