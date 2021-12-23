#pragma once
#include "NormalFloor.h"

class BreakFloor :
    public NormalFloor
{
private:
	static InstancingObjectDraw instancingObjectDraw;
	static ObjModel modelBox;
	bool isBreak;
	bool onFloor;
	bool prevOnFloor;

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
	/// 描画終了関数
	/// </summary>
	static void EndDraw();

	/// <summary>
	/// 当たり判定更新
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// ブロックを壊す
	/// </summary>
	void Break();

	/// <summary>
	/// 上に球が乗ってるかのフラグセット
	/// </summary>
	/// <param name="onFloor"></param>
	void SetOnFloor(bool onFloor) { this->onFloor = onFloor; }

	virtual std::string GetObjectType() override {
		if (isBreak == false) return "BreakFloor";
		else return "HoleFloor";
	}
};

