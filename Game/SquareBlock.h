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
	void Initialize(float posX, float posZ, float sphereRadius) override;

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

	const std::string& GetObjectType() override { return "SquareBlock"; }
};

