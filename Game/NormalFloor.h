#pragma once
#include "BaseFloor.h"
class NormalFloor :
    public BaseFloor
{
private:
	static InstancingObjectDraw instancingObjectDraw;
	static ObjModel modelBox;

public:
	static void CreateModel();
	static void StaticInitialize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const StageVec2& pos) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw();

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;

	static void ResetIndex() { instancingObjectDraw.ResetIndex(); }

	virtual std::string GetObjectType() override { return "NormalFloor"; }
};

