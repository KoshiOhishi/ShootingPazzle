#pragma once
#include "BaseFloor.h"

//穴　床に何もないマスはこのオブジェクトが挿入される
class HoleFloor :
    public BaseFloor
{
private:

public:
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
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;
};

