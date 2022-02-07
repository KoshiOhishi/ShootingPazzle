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
};

