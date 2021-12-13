#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <fbxsdk.h>
#include "FbxLoader.h"

struct Node
{
	//���O
	std::string name;
	//���[�J���X�P�[��
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	//���[�J����]�p
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	//���[�J���ړ�
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	//���[�J���ό`�s��
	DirectX::XMMATRIX transform;
	//�O���[�o���ό`�s��
	DirectX::XMMATRIX globalTransform;
	//�e�m�[�h
	Node* parent = nullptr;
};

class FbxModel
{
public: //�T�u�N���X

	//�{�[���\����
	struct Bone
	{
		//���O
		std::string name;
		//�����p���̋t�s��
		DirectX::XMMATRIX invInitialPose;
		//�N���X�^�[(FBX���̃{�[�����)
		FbxCluster* fbxCluster;
		//�R���X�g���N�^
		Bone(const std::string name) {
			this->name = name;
		}
	};

	//�}�e���A���\����
	struct Material
	{
		//�A���x�h
		DirectX::XMFLOAT3 baseColor = { 1,1,1 };
		//�����x(0 or 1)
		float metalness = 0.0f;
		//���ʔ��˓x(0 �` 1)
		float specular = 0.5f;
		//�e��
		float roughness = 0.0f;
	};

private: //�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	//std::���ȗ�
	using string = std::string;
	template<class T> using vector = std::vector<T>;

public: //�萔
	static const int MAX_BONE_INDICES = 4;

public: //�T�u�N���X
	//���_�f�[�^�\����
	struct VertexPosNormalUVSkin
	{
		DirectX::XMFLOAT3 pos;	//xyz���W
		DirectX::XMFLOAT3 normal;	//�@���x�N�g��
		DirectX::XMFLOAT2 uv;	//uv���W
		UINT boneIndex[MAX_BONE_INDICES];	//�{�[���@�ԍ�
		float boneWeight[MAX_BONE_INDICES];	//�{�[���@�d��
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;	//�A���r�G���g�W��
		float pad1;			//�p�f�B���O
		XMFLOAT3 diffuse;	//�f�B�t���[�Y�W��
		float pad2;			//�p�f�B���O
		XMFLOAT3 specular;	//�X�y�L�����[�W��
		float alpha;		//�A���t�@
	};

	//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	struct ConstBufferDataMaterial
	{
		//�A���x�h
		DirectX::XMFLOAT3 baseColor;
		//�����x
		float metalness;
		//���ʔ��˓x
		float specular;
		//�e��
		float roughness;
		//�p�f�B���O(16Byte���E)
		float pad[2];
	};

public:
	//�t�����h�N���X
	friend class FbxLoader;

public: //�����o�֐�
	~FbxModel();

	//�o�b�t�@����
	void CreateBuffers(ID3D12Device* device);

	//�`��
	void Draw();

	//�}�e���A���p�����[�^�]��
	void TransferMaterial();

	//���f���ǂݍ���(fbx)
	void CreateFromFBX(const string& modelName);

public: //Getter

	//���f���̕ό`�s��擾
	const XMMATRIX& GetModelTransForm() { return meshNode->globalTransform; }

	//�{�[���z��擾
	std::vector<Bone>& GetBones() { return bones; }

	//�V�[���擾
	FbxScene* GetFbxScene() { return fbxScene; }

	const DirectX::XMFLOAT3& GetBaseColor() { return material.baseColor; }
	float GetMetalness() { return material.metalness; }
	float GetSpecular() { return material.specular; }
	float GetRoughness() { return material.roughness; }

//Setter
	void SetBaseColor(const DirectX::XMFLOAT3& _baseColor) { material.baseColor = _baseColor; }
	void SetMetalness(float _metalness) { material.metalness = _metalness; }
	void SetSpecular(float _specular) { material.specular = _specular; }
	void SetRoughness(float _roughness) { material.roughness = _roughness; }

private: //�����o�ϐ�

	//���f����
	std::string name;
	//�m�[�h�z��
	std::vector<Node> nodes;
	//���b�V�������m�[�h
	Node* meshNode = nullptr;
	//�{�[���z��
	std::vector<Bone> bones;

	//FBX�V�[��
	FbxScene* fbxScene = nullptr;

	//���_�f�[�^�z��
	std::vector<VertexPosNormalUVSkin> vertices;
	//���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;

	//�A���r�G���g�W��
	DirectX::XMFLOAT3 ambient = { 1,1,1 };
	//�f�B�t���[�Y�W��
	DirectX::XMFLOAT3 diffuse = { 1,1,1 };
	//�X�؃L�����W��
	DirectX::XMFLOAT3 specular = { 0, 0, 0 };
	//�A���t�@
	float alpha = 1;
	//�e�N�X�`�����^�f�[�^
	DirectX::TexMetadata metadata = {};
	//�X�N���b�`�C���[�W
	DirectX::ScratchImage scratchImg = {};

	//UE4�}�e���A��
	Material material;

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//�C���f���b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;
	//�萔�o�b�t�@�i�}�e���A���j
	ComPtr<ID3D12Resource> constBuffMaterial;
};

