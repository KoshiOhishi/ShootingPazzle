#pragma once
#include <Windows.h>
#include <string>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#pragma comment(lib, "d3d12.lib")

//Todo:���t���[���e�L�X�g���ς��Ƃ������d�����Ȃ�̂ŗv���P

#define DX12TEXT_QUALITY_DEFAULT 0			//�t�H���g�̕����i���͏d������܂���B
#define DX12TEXT_QUALITY_DRAFT 1			//�t�H���g�̕����i���́A PROOF_QUALITY ���g�p�����Ƃ��قǂ͏d������܂���B
#define DX12TEXT_QUALITY_PROOF 2			//�t�H���g�̕����i�����A�_���t�H���g�̑����𐳊m�Ɉ�v�����邱�Ƃ����d������܂��B
#define DX12TEXT_QUALITY_NONANTIALIASED 3	//�t�H���g�̓A���`�G�C���A�X�\������Ȃ��悤�ɂ��܂��B
#define DX12TEXT_QUALITY_ANTIALIASED 4		//�t�H���g���A���`�G�C���A�X���T�|�[�g���Ă���ꍇ�̓A���`�G�C���A�X�\���ɂȂ�܂��B
#define DX12TEXT_QUALITY_CLEARTYPE 5		//ClearType �A���`�G�C���A�X���\�b�h���g�p���ă����_�����O����܂��B

#define DX12TEXT_WEIGHT_DONTCARE 0
#define DX12TEXT_WEIGHT_THIN 100
#define DX12TEXT_WEIGHT_ULTRALIGHT 200
#define DX12TEXT_WEIGHT_LIGHT 300
#define DX12TEXT_WEIGHT_NORMAL 400
#define DX12TEXT_WEIGHT_REGULAR 500
#define DX12TEXT_WEIGHT_SEMIBOLD 600
#define DX12TEXT_WEIGHT_BOLD 700
#define DX12TEXT_WEIGHT_ULTRABOLD 800
#define DX12TEXT_WEIGHT_HEAVY 900

#define DX12TEXT_COLOR_MAX_RATE 255

struct FontData {
	int  height = 32;// �����Z���܂��͕����̍��� (�t�H���g�T�C�Y)
	int  width = 0;// ���ϕ����� (0�Ŏ�������)
	int  weight = 0;// �t�H���g�̑��� (0�`1000�A0�Ńf�t�H���g)
	bool  italic = false;// �C�^���b�N�̎w��
	//bool  underline = false;// �����t���w��
	//bool  strikeOut = false;// �ł��������t���w��
	int  quality = DX12TEXT_QUALITY_DEFAULT;// �o�͕i�� (DX12TEXT_QUALITY_...)
	std::wstring fontName = L"�l�r �S�V�b�N";// �t�H���g��
};

class RenderText
{
private:
	using string = std::string;
	using wstring = std::wstring;
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public:
	struct VertexPosUv

	{
		XMFLOAT3 pos;	//xyz���W
		XMFLOAT2 uv;	//uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMFLOAT4 color; // �F (RGBA)
		XMMATRIX mat;	//3D�ϊ��s��
	};

	struct TextureData {
		unsigned char R = 255;
		unsigned char G = 255;
		unsigned char B = 255;
		unsigned char A = 0;
	};

private:
	// ���_��
	static const int vertNum = 4;

	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature;
	//�p�C�v���C���X�e�[�g
	static ComPtr<ID3D12PipelineState> pipelineState;
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

	static int createdCount;

private:
	//�e�N�X�`���o�b�t�@
	ComPtr <ID3D12Resource> texbuff;
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	float rotation = 0.0f;		//Z�����̉�]�p
	XMVECTOR position{};			//���W
	XMMATRIX matWorld{};			//���[���h�s��
	XMFLOAT4 color = { 1,1,1,1 };	//�F
	UINT texNumber;				//�e�N�X�`���ԍ�
	XMFLOAT2 anchorpoint{};		//�A���J�[�|�C���g
	float width = 0;		//����
	float height = 0;		//�c��
	bool isDisplay = true;		//�\�����

	FontData fd = {};
	wstring prevStr;
	bool changeFD = false;
	int number = -1;

private:
	void Initialize(const wstring& str);
	void CreateFontTexture(const FontData& fontData, const wstring& str);
	bool IsReCreate(const wstring& str);
	void Update(float x, float y, const wstring& str);

public:
	void DrawStringPrivate();
	static void BeginDraw();

	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �t�H���g�����Z�b�g
	/// </summary>
	/// <param name=""></param>
	void SetFontData(const FontData& fontData);

	/// <summary>
	/// ������`��i�w�i�j
	/// </summary>
	/// <param name="x">X���W (2D)</param>
	/// <param name="y">Y���W (2D)</param>
	/// <param name="str">������</param>
	/// <param name="fontData">�ݒ�σt�H���g�f�[�^�\����</param>
	void DrawStringBG(float x, float y, const wstring& str);
	/// <summary>
	/// ������`��i�O�i�j
	/// </summary>
	/// <param name="x">X���W (2D)</param>
	/// <param name="y">Y���W (2D)</param>
	/// <param name="str">������</param>
	/// <param name="fontData">�ݒ�σt�H���g�f�[�^�\����</param>
	void DrawStringFG(float x, float y, const wstring& str);

#pragma region Setter
	/// <summary>
	/// �����F�Z�b�g (�e����0.0�`1.0)
	/// </summary>
	/// <param name="r">�Ԑ���</param>
	/// <param name="g">�ΐ���</param>
	/// <param name="b">����</param>
	/// <param name="a">�A���t�@�l</param>
	void SetColor(float r, float g, float b, float a = 1.0f);

	/// <summary>
	/// �����F�Z�b�g (�e����0�`255)
	/// </summary>
	/// <param name="r">�Ԑ���</param>
	/// <param name="g">�ΐ���</param>
	/// <param name="b">����</param>
	/// <param name="a">�A���t�@�l</param>
	void SetColorAs0To255(float r, float g, float b, float a = 255);
#pragma endregion

#pragma region Getter
	/// <summary>
	/// �����F��Ԃ� (�e����0.0�`1.0)
	/// </summary>
	/// <returns></returns>
	const XMFLOAT4& GetColor()const { return color; }

	/// <summary>
	/// �����F��Ԃ� (�e����0�`255)
	/// </summary>
	/// <returns></returns>
	const XMFLOAT4 GetColorAs0To255()const {
		XMFLOAT4 c = { color.x * 255, color.y * 255, color.z * 255, color.w * 255 };
		return c;
	}

	/// <summary>
	/// �����e�N�X�`���̉����擾
	/// </summary>
	/// <returns></returns>
	const float GetWidth() { return width; }

	/// <summary>
	/// �����e�N�X�`���̏c���擾
	/// </summary>
	/// <returns></returns>
	const float GetHeight() { return height; }
#pragma endregion
};