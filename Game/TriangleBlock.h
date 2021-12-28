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
	static InstancingObjectDraw instancingObjectDraw[4];
	//�O�p�`�̌���
	int triangleType;
	//���l�ʑ̃��f��
	static ObjModel modelTriangle[4];

public:
	static void CreateModel();

public:
	static void StaticInitialize();

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