#pragma once
#include "Block.h"

class SquareBlock :
	public Block
{
private:
	//���l�ʑ̃��f��
	static ObjModel modelBox;

public:
	static void CreateModel();

public:

	~SquareBlock() override;

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
	void Draw() override;

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

	std::string GetObjectType() override { return "SquareBlock"; }
};

