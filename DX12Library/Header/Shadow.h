#pragma once
#include "Sprite.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#pragma comment(lib, "d3d12.lib")

class Shadow:
	public Sprite
{
protected: //�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//std::���ȗ�
	using string = std::string;

private:
	static const float clearColor[4];

	//SRV�p�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDepthSRV;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff;
	//�O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

public:
	void Initialize();
	void Draw();

	/// <summary>
	/// �V�[���`��O����
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// �V�[���`��㏈��
	/// </summary>
	void PostDrawScene();

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
};

