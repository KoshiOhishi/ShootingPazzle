#pragma once
#include "Object3D.h"

static const int MAX_INSTANCECOUNT = 4096;
static const int MAX_ONCEDRAWCOUNT = 512;

//���f���ɂ�1��
class InstancingObjectDraw :
	public Object3D
{
protected:
	//FBX���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> instancingFbxRootsignature;
	//FBX�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> instancingFbxPipelinestate;
	//FBX���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> shadowInstancingFbxRootsignature;
	//FBX�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> shadowInstancingFbxPipelinestate;

	//OBJ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> instancingObjRootsignature;
	//OBJ�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> instancingObjPipelinestate;
	//OBJ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> shadowInstancingObjRootsignature;
	//OBJ�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> shadowInstancingObjPipelinestate;

	//�萔�o�b�t�@(���W�ϊ��s��p)
	ComPtr<ID3D12Resource> constBuffTransforms[8];
	//�萔�o�b�t�@(�V���h�E)
	ComPtr<ID3D12Resource> constBuffShadows[8];
	//�萔�o�b�t�@(���C�g�J����)
	ComPtr<ID3D12Resource> constBuffLightCamera;

	//�f�[�^�i�[�R���e�i
	std::vector<InstanceData> datas;

	int drawCount = 0;

public:

	struct ConstBufferDataInstancing
	{
		InstanceData data[MAX_INSTANCECOUNT];
	};

	struct ConstBufferDataInstancingLightCamera
	{
		XMMATRIX lightCamera;
	};

	struct ConstBufferDataInstancingShadow
	{
		XMMATRIX world[MAX_INSTANCECOUNT];
	};

	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̐���
	/// </summary>
	/// <param name="objectType">��������p�C�v���C�����g�p�����I�u�W�F�N�g�̎��</param>
	/// <param name="pipelineData">�p�C�v���C���ݒ�ϐ�</param>
	static void CreateGraphicsPipeline(int objectType, PipelineData& pipelineData);

	/// <summary>
	/// OBJ�V���h�E�p�p�C�v���C������
	/// </summary>
	static void CreateShadowObjGraphicsPipeline();

	/// <summary>
	/// FBX�V���h�E�p�p�C�v���C������
	/// </summary>
	static void CreateShadowFbxGraphicsPipeline();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize() override;

	/// <summary>
	/// ���t���[�������@1��̂݌Ăяo��
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="fbxModel">FBX���f��</param>
	virtual void SetFbxModel(FbxModel* fbxModel);

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="objModel">OBJ���f��</param>
	virtual void SetObjModel(ObjModel* objModel);

	/// <summary>
	/// ���̃t���[���ł͕`�悳���Ȃ��Ƃ��Ɏg�p
	/// </summary>
	void NoDraw() { drawCount = 0; }

	/// <summary>
	/// �萔�o�b�t�@�ɏ�������
	/// </summary>
	/// <param name="constBuffData"></param>
	void AddInstancingData(const InstanceData& constBuffData);

private:
	/// <summary>
	/// �`��@BeginDraw()��Y�ꂸ�ɁI
	/// </summary>
	virtual void DrawPrivate() override;

	/// <summary>
	/// �V���h�E�`��
	/// </summary>
	virtual void DrawShadow() override;
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
	void Initialize(const Vector3& pos = {}, const Vector3& rot = {}, const Vector3& scale = { 1,1,1 }, const Vector4& color = { 1,1,1,1 });

	void Update(InstancingObjectDraw& instancingObjectDraw);

	/// <summary>
	/// ���W�Z�b�g
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos) { position = pos; }

	/// <summary>
	/// ��]�p�Z�b�g
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(const Vector3& rot) { rotation = rot; }

	/// <summary>
	/// �X�P�[���Z�b�g
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) { this->scale = scale; }

	/// <summary>
	/// �F�Z�b�g
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vector4& color) { this->color = color; }

	const Vector3& GetPosition() { return position; }
	const Vector3& GetRotation() { return rotation; }
	const Vector3& GetScale() { return scale; }
	const Vector4& GetColor() { return color; }
};