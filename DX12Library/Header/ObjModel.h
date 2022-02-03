#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "Vector2.h"
#include "Vector3.h"
#pragma comment(lib, "d3d12.lib")

class ObjModel
{
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
	using string = std::string;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <typename T>
	using vector = std::vector<T>;

//�T�u�N���X
public:
	struct Vertex
	{
		Vector3 pos;	 //xyz���W
		Vector3 normal; //�@���x�N�g��
		Vector2 uv;	//uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		Vector3 ambient;	//�A���r�G���g�W��
		float pad1;			//�p�f�B���O
		Vector3 diffuse;	//�f�B�t���[�Y�W��
		float pad2;			//�p�f�B���O
		Vector3 specular;	//�X�y�L�����[�W��
		float alpha;		//�A���t�@
	};

	//�}�e���A��
	struct Material
	{
		std::string name;	//�}�e���A����
		Vector3 ambient;	//�A���r�G���g�e���x
		Vector3 diffuse;	//�f�B�t���[�Y�e���x
		Vector3 specular;	//�X�y�L�����[�e���x
		float alpha;		//�A���t�@
		std::string textureFilename;	//�e�N�X�`���t�@�C����
		//�R���X�g���N�^
		Material() {
			ambient = { 0.5f, 0.5f, 0.5f };
			diffuse = { 0.5f, 0.5f, 0.5f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

private:
	// ���_�o�b�t�@
	ComPtr < ID3D12Resource> vertBuff = nullptr;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	// �C���f�b�N�X�o�b�t�@
	ComPtr <ID3D12Resource> indexBuff = nullptr;

	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//���_�f�[�^
	vector<Vertex> vertices;

	//�C���f�b�N�X�f�[�^
	vector<unsigned short> indices;

	//�}�e���A��
	Material material;

	// �e�N�X�`���o�b�t�@
	ComPtr <ID3D12Resource> texBuff = nullptr;

	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;

	// �V�F�[�_���\�[�X�r���[�̃n���h��(GPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;

	//OBJ����ǂݍ��܂ꂽ���ۂ�
	bool isLoadFromOBJFile = true;

	//���_���W�X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;

	//�e�N�X�`���ԍ�
	UINT texNumber;

private:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	bool LoadTexture(const std::string& directoryPath, const std::string& filename);
	bool LoadTextureReturnTexSize(const std::string& directoryPath, const std::string& filename, float* texWidth = nullptr, float* texHeight = nullptr);
	void CreateBuffers();

	/// <summary>
	/// �G�b�W�������f�[�^�̒ǉ�
	/// </summary>
	/// <param name="indexPosition">���W�C���f�b�N�X</param>
	/// <param name="indexVertex">���_�C���f�b�N�X</param>
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	/// <summary>
	/// ���������ꂽ���_�@���̌v�Z
	/// </summary>
	void CalculateSmoothedVertexNormals();

//�����o�֐�
public:

	void Draw(int instancingCount = 1, bool isShadow = false);

	/// <summary>
	/// ���_�o�b�t�@�X�V
	/// </summary>
	void UpdateVertBuff();

	/// <summary>
	/// OBJ�t�@�C�����烂�f�����쐬����i�t�H���_���ƒ��g�̖��O�͓���̂��̂ɂ��Ă��������B�j
	/// </summary>
	/// <param name="modelname">obj�t�@�C�����i�[���Ă�t�H���_��</param>
	/// <param name="smoothing">�G�b�W�������t���O</param>
	void CreateFromOBJ(const std::string& modelPath, bool smoothing = false);

	/// <summary>
	/// 1���_���w�肵��UV�l�Z�b�g
	/// </summary>
	/// <param name="vertexNum"></param>
	/// <param name="adjustUV"></param>
	void SetVertexUV(int vertexNum, const Vector2& uv);

	/// <summary>
	/// OBJ�t�@�C��������ꂽ���f���ł��邩�ǂ����Ԃ�
	/// </summary>
	/// <returns></returns>
	bool GetIsOBJ() { return isLoadFromOBJFile; }

	//�ȉ��|���S���쐬
	/// <summary>
	/// �l�p�`�|���S�����쐬�B���S��B
	/// </summary>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <param name="depth">���s</param>
	/// <param name="ambient">�A���r�G���g(����)</param>
	/// <param name="diffuse">�f�B�t���[�Y(�g�U���ˌ�)</param>
	/// <param name="specular">�X�y�L�����[(���ʔ��ˌ�)</param>
	void CreateBox(float width, float height, float depth, bool smoothing = false, const Vector3& ambient = {0.5f,0.5f,0.5f}, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// ���|���S�����쐬
	/// </summary>
	/// <param name="vertexX">���̕����� (3�ȏ�)</param>
	/// <param name="vertexY">�����̕����� (3�ȏ�)</param>
	/// <param name="radius">���a</param>
	/// <param name="ambient">�A���r�G���g(����)</param>
	/// <param name="diffuse">�f�B�t���[�Y(�g�U���ˌ�)</param>
	/// <param name="specular">�X�y�L�����[(���ʔ��ˌ�)</param>
	void CreateSphere(int vertexX, int vertexY, float radius, bool smoothing = false, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// �~���|���S�����쐬
	/// </summary>
	/// <param name="vertex">��ʂ̒��_��</param>
	/// <param name="radius">���a</param>
	/// <param name="height">����</param>
	/// <param name="ambient">�A���r�G���g(����)</param>
	/// <param name="diffuse">�f�B�t���[�Y(�g�U���ˌ�)</param>
	/// <param name="specular">�X�y�L�����[(���ʔ��ˌ�)</param>
	void CreatePoll(int vertex, float radius, float height, bool smoothing = false, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// �O�p�`�|���S�����쐬
	/// </summary>
	/// <param name="p1">���_1</param>
	/// <param name="p2">���_2</param>
	/// <param name="p3">���_3</param>
	/// <param name="ambient">�A���r�G���g(����)</param>
	/// <param name="diffuse">�f�B�t���[�Y(�g�U���ˌ�)</param>
	/// <param name="specular">�X�y�L�����[(���ʔ��ˌ�)</param>
	void CreateTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// �l�p�`�|���S�����쐬
	/// </summary>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <param name="ambient">�A���r�G���g(����)</param>
	/// <param name="diffuse">�f�B�t���[�Y(�g�U���ˌ�)</param>
	/// <param name="specular">�X�y�L�����[(���ʔ��ˌ�)</param>
	void CreatePlane(float width, float height, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// �e�N�X�`������l�p�`�|���S�����쐬
	/// </summary>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <param name="texName">�e�N�X�`���̖��O</param>
	/// <param name="ambient">�A���r�G���g(����)</param>
	/// <param name="diffuse">�f�B�t���[�Y(�g�U���ˌ�)</param>
	/// <param name="specular">�X�y�L�����[(���ʔ��ˌ�)</param>
	void CreatePlaneTex(float width, float height, const string& texturePath, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

#pragma region Getter
	/// <summary>
	/// ���_�z����擾
	/// </summary>
	/// <returns>���_�z��</returns>
	inline const std::vector<Vertex>& GetVertices() { return vertices; }

	/// <summary>
	/// �C���f�b�N�X�z����擾
	/// </summary>
	/// <returns>�C���f�b�N�X�z��</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices; }
#pragma endregion
};

