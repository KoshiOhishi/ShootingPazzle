#pragma once
#include "NormalFloor.h"

class BreakFloor :
    public NormalFloor
{
private:
	static ObjModel modelBox;
	bool isBreak;
	bool onFloor;
	bool prevOnFloor;

public:
	static void CreateModel();
	static ObjModel* GetModel() { return &modelBox; }
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
	/// �����蔻��X�V
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// �u���b�N����
	/// </summary>
	void Break();

	/// <summary>
	/// ��ɋ�������Ă邩�̃t���O�Z�b�g
	/// </summary>
	/// <param name="onFloor"></param>
	void SetOnFloor(bool onFloor) { this->onFloor = onFloor; }
};