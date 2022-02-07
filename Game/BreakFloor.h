#pragma once
#include "NormalFloor.h"

class BreakFloor :
    public NormalFloor
{
private:
	//モデル
	static ObjModel modelBox;
	//パーティクル
	static Particle3D particle;

	//破壊されたか
	bool isBreak;
	//上に球が乗っているか
	bool onFloor;
	//前フレームで上に球が乗っていたか
	bool prevOnFloor;

public:
	static void StaticInitialize();
	static ObjModel* GetModel() { return &modelBox; }

	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="pos">生成位置</param>
	static void GenerateParticle(const Vector3& pos);

	/// <summary>
	/// パーティクル描画
	/// </summary>
	static void DrawParticle();

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
	/// ブロックの種類を返す
	/// </summary>
	/// <returns></returns>
	int GetFloorType() override;

	/// <summary>
	/// ブロックを壊す
	/// </summary>
	void Break();

	/// <summary>
	/// 上に球が乗ってるかのフラグセット
	/// </summary>
	/// <param name="onFloor"></param>
	void SetOnFloor(bool onFloor) { this->onFloor = onFloor; }
};