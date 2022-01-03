#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#pragma comment(lib, "d3d12.lib")

class Sprite
{
public:
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

//�T�u�N���X
public:
	struct VertexPosUv

	{
		Vector3 pos;	//xyz���W
		Vector2 uv;	//uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		Vector4 color; // �F (RGBA)
		XMMATRIX mat;	//3D�ϊ��s��
	};

//�萔
private:
	static const int spriteSRVCount;

//�ÓI�����o�ϐ�
protected:

	// ���_��
	static const int vertNum = 4;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> spriteRootSignature;
	//�p�C�v���C���X�e�[�g
	static ComPtr<ID3D12PipelineState> spritePipelineState;
	//�ˉe�s��
	static XMMATRIX matProjection;
	//���_�V�F�[�_�I�u�W�F�N�g
	static ComPtr<ID3DBlob> vsBlob;
	//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	static ComPtr<ID3DBlob> psBlob;
	//�G���[�I�u�W�F�N�g
	static ComPtr <ID3DBlob> errorBlob;
	//�f�X�N���v�^�q�[�v
	static ComPtr <ID3D12DescriptorHeap> descHeap;
	//�e�N�X�`���o�b�t�@
	static ComPtr <ID3D12Resource> texbuff[];
	//���[�h���ꂽ�e�N�X�`���̃t�@�C�����Ɣԍ���R�Â��郊�X�g
	static std::unordered_map<std::wstring, UINT> loadedTextureList;
	//���[�h���ꂽ�e�N�X�`����
	static UINT loadedTextureCount;

//�ÓI�����o�֐�
public:

	/// <summary>
	/// ����������
	/// </summary>
	static void FirstInit();

	/// <summary>
	/// �X�v���C�g�̃e�N�X�`���ǂݍ��ݏ���
	/// </summary>
	/// <param name="texnumber">�e�N�X�`���ԍ�</param>
	/// <param name="filename">�t�@�C���p�X</param>
	/// <returns>�ǂ݂��񂾉摜�̃e�N�X�`���ԍ�</returns>
	static UINT LoadTexture(const std::wstring& filename);

	/// <summary>
	/// �X�v���C�g�p�C�v���C�����Z�b�g �X�v���C�g�`��O�ɕK������
	/// </summary>
	static void BeginDraw();

//�����o�֐�
public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

	/// <summary>
	/// ���_�o�b�t�@���X�V
	/// </summary>
	void UpdateVertBuff();


//�����o�ϐ�
protected:
	float rotation = 0.0f;		//Z�����̉�]�p
	Vector2 position{};			//���W
	XMMATRIX matWorld{};			//���[���h�s��
	Vector4 color = { 1,1,1,1 };	//�F
	UINT texNumber;				//�e�N�X�`���ԍ�
	Vector2 anchorpoint{};		//�A���J�[�|�C���g
	float width = 0;		//����
	float height = 0;		//�c��
	bool isFlipX = false;		//���E����ւ�
	bool isFlipY = false;		//�㉺����ւ�
	float tex_x = 0;		//�摜�؂�o��X���W
	float tex_y = 0;		//�摜�؂�o��y���W
	float tex_width = 0;	//�摜�؂�o������
	float tex_height = 0;	//�摜�؂�o���c��
	bool isDisplay = true;		//�\�����
	ComPtr<ID3D12Resource> vertBuff;				//���_�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;				//�萔�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView{};			//���_�o�b�t�@�r���[


public:
#pragma region �Q�b�^�[
	/// <summary>
	/// ���W���擾����
	/// </summary>
	/// <returns>���W</returns>
	const Vector2& GetPosition() const { return position; }

	/// <summary>
	/// �F�����擾����
	/// </summary>
	/// <returns>�F���</returns>
	const Vector4& GetColor() const { return color; }

	/// <summary>
	/// Z������̉�]�p���擾����
	/// </summary>
	/// <returns>Z������̉�]�p</returns>
	const float GetRotation() const { return rotation; }

	/// <summary>
	/// �X�P�[�����擾����
	/// </summary>
	/// <returns>�X�P�[��</returns>
	const Vector2& GetScale() { return { width,height }; }

	/// <summary>
	/// �����]�t���O���擾����
	/// </summary>
	/// <returns>�����]�t���O</returns>
	const bool GetIsFlipX() { return isFlipX; }

	/// <summary>
	/// �c���]�t���O���擾����
	/// </summary>
	/// <returns>�c���]�t���O</returns>
	const bool GetIsFlipY() { return isFlipY; }

	/// <summary>
	/// �\���t���O���擾����
	/// </summary>
	/// <returns>�\���t���O</returns>
	const bool GetIsDisplay() { return isDisplay; }

#pragma endregion

#pragma region �Z�b�^�[
	/// <summary>
	/// ���W���Z�b�g����
	/// </summary>
	/// <param name="pos">���W</param>
	void SetPosition(const Vector2& pos);

	/// <summary>
	/// �F�����Z�b�g����
	/// </summary>
	/// <param name="color">�F���</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// Z������̉�]�p���Z�b�g����
	/// </summary>
	/// <param name="rotation">Z������̉�]�p</param>
	void SetRotation(const float rotation);

	/// <summary>
	/// �X�P�[�����Z�b�g����
	/// </summary>
	/// <param name="scale">�X�P�[��</param>
	void SetScale(const Vector2& scale);

	/// <summary>
	/// �����]�t���O���Z�b�g����
	/// </summary>
	/// <param name="isFlipX">�����]�t���O</param>
	void SetIsFlipX(const bool isFlipX);

	/// <summary>
	/// �c���]�t���O���Z�b�g����
	/// </summary>
	/// <param name="isFlipY">�c���]�t���O</param>
	void SetIsFlipY(const bool isFlipY);

	/// <summary>
	/// �\���t���O���Z�b�g����
	/// </summary>
	/// <param name="isDisplay">�\���t���O</param>
	void SetIsDisplay(const bool isDisplay);

	/// <summary>
	/// �摜���Z�b�g
	/// </summary>
	/// <param name="filename">�t�@�C����</param>
	/// <param name="loadNewerIfNotFound">�摜��������Ȃ������Ƃ��ɐV�����ǂݍ��ނ��ǂ���</param>
	void SetTexture(const std::wstring& filename, const bool loadNewerIfNotFound = true);

	/// <summary>
	/// �ŏ��ɐݒ肳���ׂ��l�̃Z�b�g
	/// </summary>
	/// <param name="posX">X���W</param>
	/// <param name="posY">Y���W</param>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	void SetInitParams(const float posX, const float posY, const float width, const float height);

	/// <summary>
	/// �`��͈�(���N�^���O��)���Z�b�g
	/// </summary>
	/// <param name="tex_x">�؂���x���W</param>
	/// <param name="tex_y">�؂���y���W</param>
	/// <param name="tex_width">�؂��艡��(px)</param>
	/// <param name="tex_height">	�؂���c��(px)</param>
	void SetDrawRectangle(const float tex_x, const float tex_y, const float tex_width, const float tex_height);

	/// <summary>
	/// �A���J�[�|�C���g���Z�b�g����
	/// </summary>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	void SetAnchorpoint(const Vector2& anchorpoint);
#pragma endregion
};

/*
	ComPtr<ID3D12RootSignature> Sprite::spriteRootSignature = nullptr;	//���[�g�V�O�l�`��
	ComPtr<ID3D12PipelineState> Sprite::spritePipelineState = nullptr;	//�p�C�v���C���X�e�[�g
	ComPtr<ID3D12Resource> Sprite::texbuff = nullptr;					//�e�N�X�`���o�b�t�@
	ComPtr<ID3DBlob> Sprite::vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> Sprite::psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> Sprite::errorBlob = nullptr; // �G���[�I�u�W�F�N�g
	ComPtr<ID3D12DescriptorHeap> Sprite::basicDescHeap = nullptr;
	XMMATRIX Sprite::spriteMatProjection;		//�ˉe�s��

*/

