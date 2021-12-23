#pragma once
#include "BaseFloor.h"

//���@���ɉ����Ȃ��}�X�͂��̃I�u�W�F�N�g���}�������
class HoleFloor :
    public BaseFloor
{
private:

public:
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

	virtual std::string GetObjectType() override { return "HoleFloor"; }
};

