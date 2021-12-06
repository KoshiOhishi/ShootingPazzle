#pragma once
#include "Block.h"

enum ShapeType {
	SHAPETYPE_NO_LEFTTOP,	//����̊p���Ȃ��O�p�`
	SHAPETYPE_NO_RIGHTTOP,	//�E��̊p���Ȃ��O�p�`
	SHAPETYPE_NO_LEFTBOTTOM,	//�����̊p���Ȃ��O�p�`
	SHAPETYPE_NO_RIGHTBOTTOM,	//�E���̊p���Ȃ��O�p�`
};

class TriangleBlock :
    public Block
{
private:
	//�O�p�`�̌���
	int shapeType;
	//���l�ʑ̃��f��
	static ObjModel modelTriangle;

public:
	static void CreateModel();

public:

	~TriangleBlock() override;

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(float posX, float posZ, float sphereRadius) override;

	/// <summary>
	/// �O�p�`�̌`���Z�b�g
	/// </summary>
	/// <param name="syapeType"></param>
	void SetShapeType(int shapeType);

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

	std::string GetObjectType() override { return "TriangleBlock_" + std::to_string(shapeType); }
};