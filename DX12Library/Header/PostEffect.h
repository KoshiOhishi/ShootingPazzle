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
	static const int RENDERCOUNT = 1;
	static const float clearColor[4];

public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �V�[���`��O����
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// �V�[���`��㏈��
	/// </summary>
	void PostDrawScene();

	/// <summary>
	/// �`��R�}���h�̔��s
	/// </summary>
	void Draw();

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
	//DSV�쐬
	void CreateDSV();
	//�p�C�v���C������
	void CreateGraphicsPipelineState();

private: //�����o�ϐ�

	//�萔�o�b�t�@�i���ԁj
	ComPtr<ID3D12Resource> constBuffTime;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff[RENDERCOUNT];
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//�O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	//����
	float time = 0;

public:

	void SetTime(float time) { this->time = time; }
};

