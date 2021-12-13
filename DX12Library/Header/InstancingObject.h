#pragma once
#include "Object3D.h"

class InstancingObject :
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

public:
	static const int MAX_INSTANCECOUNT = 512;

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
	virtual void Initialize() override;

	/// <summary>
	/// ���t���[�������@1��̂݌Ăяo��
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// �`��@1��̂݌Ăяo��
	/// </summary>
	virtual void Draw() override;

	/// <summary>
	/// �o�b�t�@�X�V�@����������[�v������
	/// </summary>
	/// <param name="index">�C���f�b�N�X</param>
	void UpdateBuffer(int index);
};

