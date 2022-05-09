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

namespace DX12Library {

	enum SpriteBlendMode {
		SPRITE_BLENDMODE_NORMAL,	//�ʏ�`��
		SPRITE_BLENDMODE_ADD,		//���Z����
		SPRITE_BLENDMODE_SUB,		//���Z����
		SPRITE_BLENDMODE_MUL,		//��Z����
		SPRITE_BLENDMODE_SCREEN,	//�X�N���[������
		SPRITE_BLENDMODE_REVERSE	//���]
	};

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
		static const int PIPELINE_COUNT = 6;

		//�ÓI�����o�ϐ�
	protected:

		// ���_��
		static const int vertNum = 4;
		//���[�g�V�O�l�`��
		static ComPtr<ID3D12RootSignature> spriteRootSignature;
		//�p�C�v���C���X�e�[�g
		static ComPtr<ID3D12PipelineState> spritePipelineState[PIPELINE_COUNT];
		//�ˉe�s��
		static XMMATRIX matProjection;
		//�f�X�N���v�^�q�[�v
		static ComPtr <ID3D12DescriptorHeap> descHeap;
		//�e�N�X�`���o�b�t�@
		static ComPtr <ID3D12Resource> texbuff[];
		//���[�h���ꂽ�e�N�X�`���̃t�@�C�����Ɣԍ���R�Â��郊�X�g
		static std::unordered_map<std::string, UINT> loadedTextureList;
		//���[�h���ꂽ�e�N�X�`����
		static UINT loadedTextureCount;

		//�����o�ϐ�
	protected:
		Vector2 position{};					//���W
		float rotation = 0.0f;				//Z�����̉�]�p
		Vector4 color = { 1,1,1,1 };		//�F
		UINT texNumber;						//�e�N�X�`���ԍ�
		Vector2 anchorpoint{};				//�A���J�[�|�C���g
		Vector2 size;						//�摜�\���T�C�Y
		float width = 0;					//�ǂ݂��񂾃f�[�^�̉���
		float height = 0;					//�ǂ݂��񂾃f�[�^�̏c��
		bool isFlipX = false;				//���E����ւ�
		bool isFlipY = false;				//�㉺����ւ�
		float tex_x = 0;					//�摜�؂�o��X���W
		float tex_y = 0;					//�摜�؂�o��y���W
		float tex_width = 0;				//�摜�؂�o������
		float tex_height = 0;				//�摜�؂�o���c��
		bool isDisplay = true;				//�\�����
		XMMATRIX matWorld{};				//���[���h�s��
		ComPtr<ID3D12Resource> vertBuff;	//���_�o�b�t�@
		ComPtr<ID3D12Resource> constBuff;	//�萔�o�b�t�@
		D3D12_VERTEX_BUFFER_VIEW vbView{};	//���_�o�b�t�@�r���[
		int pipelineIndex = 0;				//�g�p����p�C�v���C���̃C���f�b�N�X

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
		/// <param name="filepath">�t�@�C���p�X</param>
		/// <returns>�ǂ݂��񂾉摜�̃e�N�X�`���ԍ�</returns>
		static UINT LoadTexture(const std::string& filepath);

		//�����o�֐�
	public:

		/// <summary>
		/// ������
		/// </summary>
		void Initialize();

		/// <summary>
		/// �w�i�ɕ`��
		/// </summary>
		void DrawBG();

		/// <summary>
		/// �O�i�ɕ`��
		/// </summary>
		void DrawFG();

		/// <summary>
		/// �X�v���C�g�p�C�v���C�����Z�b�g �X�v���C�g�`��O�ɕK������
		/// </summary>
		static void BeginDraw();

		/// <summary>
		/// �`�揈��
		/// </summary>
		void DrawPrivate();

	private:

		/// <summary>
		/// ���_�o�b�t�@���X�V
		/// </summary>
		void UpdateVertBuff();

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
		/// �X�v���C�g�\���T�C�Y���擾����
		/// </summary>
		/// <returns>�摜�T�C�Y</returns>
		const Vector2& GetSize()const { return size; }

		/// <summary>
		/// �ǂ݂��񂾉摜�f�[�^�̃T�C�Y���擾����
		/// </summary>
		/// <returns>�摜�T�C�Y</returns>
		const Vector2& GetTexSize()const { return { width,height }; }

		/// <summary>
		/// �����]�t���O���擾����
		/// </summary>
		/// <returns>�����]�t���O</returns>
		const bool GetIsFlipX()const { return isFlipX; }

		/// <summary>
		/// �c���]�t���O���擾����
		/// </summary>
		/// <returns>�c���]�t���O</returns>
		const bool GetIsFlipY()const { return isFlipY; }

		/// <summary>
		/// �\���t���O���擾����
		/// </summary>
		/// <returns>�\���t���O</returns>
		const bool GetIsDisplay()const { return isDisplay; }

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
		/// �X�P�[�����Z�b�g����(2�������l 1920x1080�Ȃ�)
		/// </summary>
		/// <param name="scale">�X�P�[��</param>
		void SetSize(const Vector2& scale);

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
		void SetTexture(const std::string& filepath, const bool loadNewerIfNotFound = true);

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

		/// <summary>
		/// �`��̃u�����h�ݒ���Z�b�g
		/// </summary>
		/// <param name="drawMode">�u�����h�ݒ�(SPRITE_BLENDMODE_...)</param>
		void SetBlendMode(int blendMode) {
			//�͈͊O�Ȃ�Ȃɂ����Ȃ�
			if (blendMode < 0 || blendMode >= PIPELINE_COUNT) {
				return;
			}
			pipelineIndex = blendMode;
		}
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

}