#pragma once
#include "Object3D.h"

static const int MAX_INSTANCECOUNT = 512;

//���f���ɂ�1��
class InstancingObjectDraw :
	public Object3D
{
protected:
	//FBX���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> instancingFbxRootsignature;
	//FBX�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> instancingFbxPipelinestate;

	//OBJ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> instancingObjRootsignature;
	//OBJ�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> instancingObjPipelinestate;

	int index = 0;
public:

	struct ConstBufferDataInstancing
	{
		InstanceData data[MAX_INSTANCECOUNT];
	};

	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̐���
	/// </summary>
	/// <param name="objectType">��������p�C�v���C�����g�p�����I�u�W�F�N�g�̎��</param>
	/// <param name="pipelineData">�p�C�v���C���ݒ�ϐ�</param>
	static void CreateGraphicsPipeline(int objectType, PipelineData& pipelineData);

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// ���t���[�������@1��̂݌Ăяo��
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��@1��̂݌Ăяo��
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// �萔�o�b�t�@�ɏ�������
	/// </summary>
	/// <param name="constBuffData"></param>
	void UpdateConstBuff(const InstanceData& constBuffData);

	/// <summary>
	/// �`��֐���ʂ�Ȃ��Ƃ��͂��������
	/// </summary>
	void ResetIndex() { index = 0; }

	Microsoft::WRL::ComPtr<ID3D12Resource> GetConstBuff() { return constBuffTransform; }
};

//�e�̂�1��
class InstancingObject
{
private:

	//���[�J���X�P�[��
	Vector3 scale = { 1,1,1 };
	//X,Y,Z�����̃��[�J����]�p
	Vector3 rotation = { 0,0,0 };
	//���[�J�����W
	Vector3 position = { 0,0,0 };
	//�F
	Vector4 color = { 1,1,1,1 };
	//���[�J�����[���h�ϊ��s��
	DirectX::XMMATRIX matWorld;

public:
	void Initialize(const Vector3& pos = {}, const Vector3& rot = {}, const Vector3& scale = {1,1,1}, const Vector4& color = {1,1,1,1});
	void Update(InstancingObjectDraw& instancingObjectDraw);

	void SetPosition(const Vector3& pos) { position = pos; }
	void SetRotation(const Vector3& rot) { rotation = rot; }
	void SetScale(const Vector3& scale) { this->scale = scale; }
	void SetColor(const Vector4& color) { this->color = color; }

	const Vector3& GetPosition() { return position; }
	const Vector3& GetRotation() { return rotation; }
	const Vector3& GetScale() { return scale; }
	const Vector4& GetColor() { return color; }
};