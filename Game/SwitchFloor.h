#pragma once
#include "BaseFloor.h"

enum SwitchColor
{
	SWITCH_COLOR_NONE,
	SWITCH_COLOR_RED,
	SWITCH_COLOR_BLUE,
	SWITCH_COLOR_YELLOW,
	SWITCH_COLOR_GREEN,
};

enum SwitchState
{
	SWITCH_STATE_OFF,
	SWITCH_STATE_ON,
};

class SwitchFloor :
    public BaseFloor
{
private:
	static InstancingObjectDraw instancingObjectDraw[2];
	static ObjModel modelBox[2];
	int switchColor = SWITCH_COLOR_NONE;
	int switchState = SWITCH_STATE_OFF;

	Timer colorTimer;

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
	/// 現在のステージの色によってモデルを更新
	/// </summary>
	void UpdateSwitchState();

	/// <summary>
	/// スイッチ色更新
	/// </summary>
	void UpdateSwitchColor();

	/// <summary>
	/// スイッチの状態取得
	/// </summary>
	/// <returns>ON/OFF</returns>
	int GetSwitchState() { return switchState; }

	/// <summary>
	/// スイッチの状態セット
	/// </summary>
	/// <param name="switchState">SWITCH_STATE...</param>
	void SetSwitchState(int switchState) { this->switchState = switchState; }

	/// <summary>
	/// スイッチの色セット
	/// </summary>
	/// <param name="switchColor">SWITCH_COLOR...</param>
	void SetSwitchColor(int switchColor);
};

