#pragma once
#include "BaseBlock.h"

class SquareBlock :
	public BaseBlock
{
private:
	//���l�ʑ̃��f��
	static ObjModel modelBox[4];
	//�p�[�e�B�N��(�e�F)
	static Particle3D particle[5];

public:
	static void StaticInitialize();
	static ObjModel* GetModel(int index) { return &modelBox[index]; }

	/// <summary>
	/// �p�[�e�B�N������
	/// </summary>
	/// <param name="pos">�����ʒu</param>
	/// <param name="colorIndex">�F�C���f�b�N�X</param>
	static void GenerateParticle(int num, const Vector3& pos, int colorIndex);

	/// <summary>
	/// �p�[�e�B�N���`��
	/// </summary>
	static void DrawParticle();

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
	/// �u���b�N�̎�ނ�Ԃ�
	/// </summary>
	/// <returns></returns>
	int GetBlockType() override;

	/// <summary>
	/// ����܂ł̃J�E���g��1���炷 0�ɂȂ�����Breakup()���Ă�
	/// </summary>
	void DecrementBreakupCount() override;

	/// <summary>
	/// �u���b�N����
	/// </summary>
	void Breakup() override;

};

