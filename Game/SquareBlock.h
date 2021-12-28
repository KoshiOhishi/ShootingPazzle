#pragma once
#include "BaseBlock.h"

class SquareBlock :
	public BaseBlock
{
private:
	static InstancingObjectDraw instancingObjectDraw[4];
	//���l�ʑ̃��f��
	static ObjModel modelBox[4];

public:
	static void CreateModel();

public:
	static void StaticInitialize();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(const StageVec2& pos, float sphereRadius) override;

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
	/// ����܂ł̃J�E���g��1���炷 0�ɂȂ�����Breakup()���Ă�
	/// </summary>
	void DecrementBreakupCount() override;

	/// <summary>
	/// �u���b�N����
	/// </summary>
	void Breakup() override;
};

