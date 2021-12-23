#pragma once
#include "BaseFloor.h"
class NormalFloor :
    public BaseFloor
{
private:
	static InstancingObjectDraw instancingObjectDraw;
	static ObjModel modelBox;

public:
	static void CreateModel();
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

	virtual std::string GetObjectType() override { return "NormalFloor"; }
};

