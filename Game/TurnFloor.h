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
	int turnType;

public:
	static void CreateModel();

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
	void Draw() override;

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	void UpdateCollision() override;

	virtual std::string GetObjectType() override { return "TurnFloor_" + std::to_string(turnType); }

	/// <summary>
	/// �����]���̌����w��
	/// </summary>
	/// <param name="turnType">TURNTYPE_...</param>
	void SetTurnType(int turnType);
};

