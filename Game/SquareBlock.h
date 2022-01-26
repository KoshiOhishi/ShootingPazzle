#pragma once
#include "BaseBlock.h"

class SquareBlock :
	public BaseBlock
{
private:
	//���l�ʑ̃��f��
	static ObjModel modelBox[4];

public:
	static void StaticInitialize();
	static ObjModel* GetModel(int index) { return &modelBox[index]; }

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(const StageVec2& pos, float sphereRadius) override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// ����܂ł̃J�E���g��1���炷 0�ɂȂ�����Breakup()���Ă�
	/// </summary>
	void DecrementBreakupCount() override;

	/// <summary>
	/// �u���b�N����
	/// </summary>
	void Breakup() override;
};

