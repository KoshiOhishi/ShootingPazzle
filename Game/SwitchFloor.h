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
	static ObjModel modelBox[2];
	int switchColor = SWITCH_COLOR_NONE;
	int switchState = SWITCH_STATE_OFF;

	float addColorTime;

public:
	static void StaticInitialize();
	static ObjModel* GetModel(int index) { return &modelBox[index]; }

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(const StageVec2& pos) override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// �u���b�N�̎�ނ�Ԃ�
	/// </summary>
	/// <returns></returns>
	int GetFloorType() override;

	/// <summary>
	/// ���݂̃X�e�[�W�̐F�ɂ���ă��f�����X�V
	/// </summary>
	void UpdateSwitchState();

	/// <summary>
	/// �X�C�b�`�F�X�V
	/// </summary>
	void UpdateSwitchColor();

	/// <summary>
	/// �X�C�b�`�̏�Ԏ擾
	/// </summary>
	/// <returns>ON/OFF</returns>
	int GetSwitchState() { return switchState; }

	/// <summary>
	/// �X�C�b�`�̏�ԃZ�b�g
	/// </summary>
	/// <param name="switchState">SWITCH_STATE...</param>
	void SetSwitchState(int switchState) { this->switchState = switchState; }

	/// <summary>
	/// �X�C�b�`�̐F�Z�b�g
	/// </summary>
	/// <param name="switchColor">SWITCH_COLOR...</param>
	void SetSwitchColor(int switchColor);
};

