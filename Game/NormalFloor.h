#pragma once
#include "BaseFloor.h"
class NormalFloor :
    public BaseFloor
{
private:
	static ObjModel modelBox;

public:
	static void StaticInitialize();
	static ObjModel* GetModel() { return &modelBox; }

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
};

