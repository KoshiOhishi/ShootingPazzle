#pragma once
#include "BaseFloor.h"

enum TurnType
{
	TURNTYPE_LEFT,
	TURNTYPE_RIGHT,
	TURNTYPE_UP,
	TURNTYPE_DOWN
};

class TurnFloor :
    public BaseFloor
{
private:
	static ObjModel modelBox[4];
	static ObjModel modelCover[4];

	InstancingObject objCover;
	int turnType;

public:
	static void CreateModel();
	static ObjModel* GetModel(int index) {
		if (index < 4)	{ return &modelBox[index]; }
		else { return &modelCover[index - 4]; }
	}

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(const StageVec2& pos) override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	void UpdateCollision() override;

	/// <summary>
	/// �����]���̌����w��
	/// </summary>
	/// <param name="turnType">TURNTYPE_...</param>
	void SetTurnType(int turnType);

	/// <summary>
	/// �u���b�N��UV�𓮂���
	/// </summary>
	void RollUV();
};

