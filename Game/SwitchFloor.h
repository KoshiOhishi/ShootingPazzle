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
	/// �`��I���֐�
	/// </summary>
	static void EndDraw();

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	void UpdateCollision() override;

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

