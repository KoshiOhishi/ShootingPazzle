#pragma once
#include "BaseFloor.h"

//���@���ɉ����Ȃ��}�X�͂��̃I�u�W�F�N�g���}�������
class HoleFloor :
    public BaseFloor
{
private:

public:

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

