#pragma once
#include "BaseFloor.h"

enum SwitchColor
{
	SWITCH_COLOR_RED,
	SWITCH_COLOR_BLUE,
	SWITCH_COLOR_YELLOW,
	SWITCH_COLOR_GREEN,
};

class SwitchFloor :
    public BaseFloor
{
private:
	static InstancingObjectDraw instancingObjectDraw;
	static ObjModel modelBox;
	int switchColor;

public:
	static void CreateModel();

	static void StaticInitialize();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(const StageVec2& pos) override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	static void Draw();

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	void UpdateCollision() override;

	virtual std::string GetObjectType() override { return "SwitchFloor_" + std::to_string(switchColor); }

	/// <summary>
	/// �X�C�b�`�̐F�Z�b�g
	/// </summary>
	/// <param name="switchColor">SWITCH_COLOR...</param>
	void SetSwitchColor(int switchColor) { this->switchColor = switchColor; }

	static void ResetIndex() { instancingObjectDraw.ResetIndex(); }

};

