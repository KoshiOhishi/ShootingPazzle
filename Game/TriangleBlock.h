#pragma once
#include "BaseBlock.h"

enum TriangleType {
	TRIANGLETYPE_NO_LEFTTOP,	//����̊p���Ȃ��O�p�`
	TRIANGLETYPE_NO_RIGHTTOP,	//�E��̊p���Ȃ��O�p�`
	TRIANGLETYPE_NO_LEFTBOTTOM,	//�����̊p���Ȃ��O�p�`
	TRIANGLETYPE_NO_RIGHTBOTTOM,	//�E���̊p���Ȃ��O�p�`
};

class TriangleBlock :
    public BaseBlock
{
private:
	//�O�p�`�̌���
	int triangleType;
	//���l�ʑ̃��f��
	static DX12Library::ObjModel modelTriangle[4];
	//�p�[�e�B�N�� (�e�F)
	static DX12Library::Particle3D particle[5];

public:
	static void StaticInitialize();
	static DX12Library::ObjModel* GetModel(int index) { return &modelTriangle[index]; }
	/// <summary>
	/// �p�[�e�B�N������
	/// </summary>
	/// <param name="pos">�����ʒu</param>
	/// <param name="colorIndex">�F�C���f�b�N�X</param>
	static void GenerateParticle(int num, const DX12Library::Vector3& pos, int colorIndex);

	/// <summary>
	/// �p�[�e�B�N���`��
	/// </summary>
	static void DrawParticle();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(const StageVec2& pos, float sphereRadius) override;

	/// <summary>
	/// �O�p�`�̌`���Z�b�g
	/// </summary>
	/// <param name="syapeType"></param>
	void SetTriangleType(int shapeType);

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