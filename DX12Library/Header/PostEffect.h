#pragma once
#include "Sprite.h"

class PostEffect : 
	public Sprite
{
public: //�T�u�N���X
	struct ConstBufferDataTime 
	{
		float time;
	};

private:
	static const int RENDERCOUNT = 2;
	static const float clearColor[4];

public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(bool isDispDepthTex = false);

	/// <summary>
	/// �V�[���`��O����
	/// </summary>
	static void PreDrawScene();

	/// <summary>
	/// �V�[���`��㏈��
	/// </summary>
	static void PostDrawScene();

	/// <summary>
	/// �`��R�}���h�̔��s
	/// </summary>
	void Draw();

private:
	//VBV�쐬
	void CreateVBV();
	//�萔�o�b�t�@����
	void CreateConstBuffer();
	//�e�N�X�`������
	void CreateTexture();
	//SRV�쐬
	void CreateSRV();
	//RTV�쐬
	void CreateRTV();
	//�p�C�v���C������
	void CreateGraphicsPipelineState(bool isDispDepthTex = false);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetRtvH(int targetNum);

private: //�����o�ϐ�
	//�e�N�X�`���o�b�t�@
	static ComPtr<ID3D12Resource> texBuff[RENDERCOUNT];
	//RTV�p�f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeapRTV;

	//�萔�o�b�t�@�i���ԁj
	ComPtr<ID3D12Resource> constBuffTime;
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//�O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	//����
	float time;

public:

	void SetTime(float time) { PostEffect::time = time; }
};

