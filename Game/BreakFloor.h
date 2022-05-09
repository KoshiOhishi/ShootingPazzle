#pragma once
#include "NormalFloor.h"

class BreakFloor :
    public NormalFloor
{
private:
	//���f��
	static DX12Library::ObjModel modelBox;
	//�p�[�e�B�N��
	static DX12Library::Particle3D particle;

	//�j�󂳂ꂽ��
	bool isBreak;
	//��ɋ�������Ă��邩
	bool onFloor;
	//�O�t���[���ŏ�ɋ�������Ă�����
	bool prevOnFloor;

public:
	static void StaticInitialize();
	static DX12Library::ObjModel* GetModel() { return &modelBox; }

	/// <summary>
	/// �p�[�e�B�N������
	/// </summary>
	/// <param name="pos">�����ʒu</param>
	static void GenerateParticle(const DX12Library::Vector3& pos);

	/// <summary>
	/// �p�[�e�B�N���`��
	/// </summary>
	static void DrawParticle();

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
	/// �u���b�N�̎�ނ�Ԃ�
	/// </summary>
	/// <returns></returns>
	int GetFloorType() override;

	/// <summary>
	/// �u���b�N����
	/// </summary>
	void Break();

	/// <summary>
	/// ��ɋ�������Ă邩�̃t���O�Z�b�g
	/// </summary>
	/// <param name="onFloor"></param>
	void SetOnFloor(bool onFloor) { this->onFloor = onFloor; }
};