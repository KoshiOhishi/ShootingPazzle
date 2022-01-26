#pragma once
#include <Windows.h>
#include <string>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#pragma comment(lib, "d3d12.lib")

//Todo:毎フレームテキストが変わるとすごい重たくなるので要改善

#define DX12TEXT_QUALITY_DEFAULT 0			//フォントの文字品質は重視されません。
#define DX12TEXT_QUALITY_DRAFT 1			//フォントの文字品質は、 PROOF_QUALITY を使用したときほどは重視されません。
#define DX12TEXT_QUALITY_PROOF 2			//フォントの文字品質が、論理フォントの属性を正確に一致させることよりも重視されます。
#define DX12TEXT_QUALITY_NONANTIALIASED 3	//フォントはアンチエイリアス表示されないようにします。
#define DX12TEXT_QUALITY_ANTIALIASED 4		//フォントがアンチエイリアスをサポートしている場合はアンチエイリアス表示になります。
#define DX12TEXT_QUALITY_CLEARTYPE 5		//ClearType アンチエイリアスメソッドを使用してレンダリングされます。

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
	int  height = 32;// 文字セルまたは文字の高さ (フォントサイズ)
	int  width = 0;// 平均文字幅 (0で自動調整)
	int  weight = 0;// フォントの太さ (0～1000、0でデフォルト)
	bool  italic = false;// イタリック体指定
	//bool  underline = false;// 下線付き指定
	//bool  strikeOut = false;// 打ち消し線付き指定
	int  quality = DX12TEXT_QUALITY_DEFAULT;// 出力品質 (DX12TEXT_QUALITY_...)
	std::wstring fontName = L"ＭＳ ゴシック";// フォント名
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
		XMFLOAT3 pos;	//xyz座標
		XMFLOAT2 uv;	//uv座標
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		XMFLOAT4 color; // 色 (RGBA)
		XMMATRIX mat;	//3D変換行列
	};

	struct TextureData {
		unsigned char R = 255;
		unsigned char G = 255;
		unsigned char B = 255;
		unsigned char A = 0;
	};

private:
	// 頂点数
	static const int vertNum = 4;

	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootSignature;
	//パイプラインステート
	static ComPtr<ID3D12PipelineState> pipelineState;
	//射影行列
	static XMMATRIX matProjection;
	//頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> vsBlob;
	//ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> psBlob;
	//エラーオブジェクト
	static ComPtr <ID3DBlob> errorBlob;
	//デスクリプタヒープ
	static ComPtr <ID3D12DescriptorHeap> descHeap;

	static int createdCount;

private:
	//テクスチャバッファ
	ComPtr <ID3D12Resource> texbuff;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	float rotation = 0.0f;		//Z軸回りの回転角
	XMVECTOR position{};			//座標
	XMMATRIX matWorld{};			//ワールド行列
	XMFLOAT4 color = { 1,1,1,1 };	//色
	UINT texNumber;				//テクスチャ番号
	XMFLOAT2 anchorpoint{};		//アンカーポイント
	float width = 0;		//横幅
	float height = 0;		//縦幅
	bool isDisplay = true;		//表示状態

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
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// フォント情報をセット
	/// </summary>
	/// <param name=""></param>
	void SetFontData(const FontData& fontData);

	/// <summary>
	/// 文字列描画（背景）
	/// </summary>
	/// <param name="x">X座標 (2D)</param>
	/// <param name="y">Y座標 (2D)</param>
	/// <param name="str">文字列</param>
	/// <param name="fontData">設定済フォントデータ構造体</param>
	void DrawStringBG(float x, float y, const wstring& str);
	/// <summary>
	/// 文字列描画（前景）
	/// </summary>
	/// <param name="x">X座標 (2D)</param>
	/// <param name="y">Y座標 (2D)</param>
	/// <param name="str">文字列</param>
	/// <param name="fontData">設定済フォントデータ構造体</param>
	void DrawStringFG(float x, float y, const wstring& str);

#pragma region Setter
	/// <summary>
	/// 文字色セット (各成分0.0～1.0)
	/// </summary>
	/// <param name="r">赤成分</param>
	/// <param name="g">緑成分</param>
	/// <param name="b">青成分</param>
	/// <param name="a">アルファ値</param>
	void SetColor(float r, float g, float b, float a = 1.0f);

	/// <summary>
	/// 文字色セット (各成分0～255)
	/// </summary>
	/// <param name="r">赤成分</param>
	/// <param name="g">緑成分</param>
	/// <param name="b">青成分</param>
	/// <param name="a">アルファ値</param>
	void SetColorAs0To255(float r, float g, float b, float a = 255);
#pragma endregion

#pragma region Getter
	/// <summary>
	/// 文字色を返す (各成分0.0～1.0)
	/// </summary>
	/// <returns></returns>
	const XMFLOAT4& GetColor()const { return color; }

	/// <summary>
	/// 文字色を返す (各成分0～255)
	/// </summary>
	/// <returns></returns>
	const XMFLOAT4 GetColorAs0To255()const {
		XMFLOAT4 c = { color.x * 255, color.y * 255, color.z * 255, color.w * 255 };
		return c;
	}

	/// <summary>
	/// 文字テクスチャの横幅取得
	/// </summary>
	/// <returns></returns>
	const float GetWidth() { return width; }

	/// <summary>
	/// 文字テクスチャの縦幅取得
	/// </summary>
	/// <returns></returns>
	const float GetHeight() { return height; }
#pragma endregion
};