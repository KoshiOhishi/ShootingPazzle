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
		SPRITE_BLENDMODE_NORMAL,	//通常描画
		SPRITE_BLENDMODE_ADD,		//加算合成
		SPRITE_BLENDMODE_SUB,		//減算合成
		SPRITE_BLENDMODE_MUL,		//乗算合成
		SPRITE_BLENDMODE_SCREEN,	//スクリーン合成
		SPRITE_BLENDMODE_REVERSE	//反転
	};

	class Sprite
	{
	public:
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		//サブクラス
	public:
		struct VertexPosUv

		{
			Vector3 pos;	//xyz座標
			Vector2 uv;	//uv座標
		};

		// 定数バッファ用データ構造体
		struct ConstBufferData {
			Vector4 color; // 色 (RGBA)
			XMMATRIX mat;	//3D変換行列
		};

		//定数
	private:
		static const int spriteSRVCount;
		static const int PIPELINE_COUNT = 6;

		//静的メンバ変数
	protected:

		// 頂点数
		static const int vertNum = 4;
		//ルートシグネチャ
		static ComPtr<ID3D12RootSignature> spriteRootSignature;
		//パイプラインステート
		static ComPtr<ID3D12PipelineState> spritePipelineState[PIPELINE_COUNT];
		//射影行列
		static XMMATRIX matProjection;
		//デスクリプタヒープ
		static ComPtr <ID3D12DescriptorHeap> descHeap;
		//テクスチャバッファ
		static ComPtr <ID3D12Resource> texbuff[];
		//ロードされたテクスチャのファイル名と番号を紐づけるリスト
		static std::unordered_map<std::string, UINT> loadedTextureList;
		//ロードされたテクスチャ数
		static UINT loadedTextureCount;

		//メンバ変数
	protected:
		Vector2 position{};					//座標
		float rotation = 0.0f;				//Z軸回りの回転角
		Vector4 color = { 1,1,1,1 };		//色
		UINT texNumber;						//テクスチャ番号
		Vector2 anchorpoint{};				//アンカーポイント
		Vector2 size;						//画像表示サイズ
		float width = 0;					//読みこんだデータの横幅
		float height = 0;					//読みこんだデータの縦幅
		bool isFlipX = false;				//左右入れ替え
		bool isFlipY = false;				//上下入れ替え
		float tex_x = 0;					//画像切り出しX座標
		float tex_y = 0;					//画像切り出しy座標
		float tex_width = 0;				//画像切り出し横幅
		float tex_height = 0;				//画像切り出し縦幅
		bool isDisplay = true;				//表示状態
		XMMATRIX matWorld{};				//ワールド行列
		ComPtr<ID3D12Resource> vertBuff;	//頂点バッファ
		ComPtr<ID3D12Resource> constBuff;	//定数バッファ
		D3D12_VERTEX_BUFFER_VIEW vbView{};	//頂点バッファビュー
		int pipelineIndex = 0;				//使用するパイプラインのインデックス

	//静的メンバ関数
	public:

		/// <summary>
		/// 初期化処理
		/// </summary>
		static void FirstInit();

		/// <summary>
		/// スプライトのテクスチャ読み込み処理
		/// </summary>
		/// <param name="texnumber">テクスチャ番号</param>
		/// <param name="filepath">ファイルパス</param>
		/// <returns>読みこんだ画像のテクスチャ番号</returns>
		static UINT LoadTexture(const std::string& filepath);

		//メンバ関数
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 背景に描画
		/// </summary>
		void DrawBG();

		/// <summary>
		/// 前景に描画
		/// </summary>
		void DrawFG();

		/// <summary>
		/// スプライトパイプラインをセット スプライト描画前に必ず書く
		/// </summary>
		static void BeginDraw();

		/// <summary>
		/// 描画処理
		/// </summary>
		void DrawPrivate();

	private:

		/// <summary>
		/// 頂点バッファを更新
		/// </summary>
		void UpdateVertBuff();

	public:
#pragma region ゲッター
		/// <summary>
		/// 座標を取得する
		/// </summary>
		/// <returns>座標</returns>
		const Vector2& GetPosition() const { return position; }

		/// <summary>
		/// 色情報を取得する
		/// </summary>
		/// <returns>色情報</returns>
		const Vector4& GetColor() const { return color; }

		/// <summary>
		/// Z軸周りの回転角を取得する
		/// </summary>
		/// <returns>Z軸周りの回転角</returns>
		const float GetRotation() const { return rotation; }

		/// <summary>
		/// スプライト表示サイズを取得する
		/// </summary>
		/// <returns>画像サイズ</returns>
		const Vector2& GetSize()const { return size; }

		/// <summary>
		/// 読みこんだ画像データのサイズを取得する
		/// </summary>
		/// <returns>画像サイズ</returns>
		const Vector2& GetTexSize()const { return { width,height }; }

		/// <summary>
		/// 横反転フラグを取得する
		/// </summary>
		/// <returns>横反転フラグ</returns>
		const bool GetIsFlipX()const { return isFlipX; }

		/// <summary>
		/// 縦反転フラグを取得する
		/// </summary>
		/// <returns>縦反転フラグ</returns>
		const bool GetIsFlipY()const { return isFlipY; }

		/// <summary>
		/// 表示フラグを取得する
		/// </summary>
		/// <returns>表示フラグ</returns>
		const bool GetIsDisplay()const { return isDisplay; }

#pragma endregion

#pragma region セッター
		/// <summary>
		/// 座標をセットする
		/// </summary>
		/// <param name="pos">座標</param>
		void SetPosition(const Vector2& pos);

		/// <summary>
		/// 色情報をセットする
		/// </summary>
		/// <param name="color">色情報</param>
		void SetColor(const Vector4& color);

		/// <summary>
		/// Z軸周りの回転角をセットする
		/// </summary>
		/// <param name="rotation">Z軸周りの回転角</param>
		void SetRotation(const float rotation);

		/// <summary>
		/// スケールをセットする(2次元数値 1920x1080など)
		/// </summary>
		/// <param name="scale">スケール</param>
		void SetSize(const Vector2& scale);

		/// <summary>
		/// 横反転フラグをセットする
		/// </summary>
		/// <param name="isFlipX">横反転フラグ</param>
		void SetIsFlipX(const bool isFlipX);

		/// <summary>
		/// 縦反転フラグをセットする
		/// </summary>
		/// <param name="isFlipY">縦反転フラグ</param>
		void SetIsFlipY(const bool isFlipY);

		/// <summary>
		/// 表示フラグをセットする
		/// </summary>
		/// <param name="isDisplay">表示フラグ</param>
		void SetIsDisplay(const bool isDisplay);

		/// <summary>
		/// 画像をセット
		/// </summary>
		/// <param name="filename">ファイル名</param>
		/// <param name="loadNewerIfNotFound">画像が見つからなかったときに新しく読み込むかどうか</param>
		void SetTexture(const std::string& filepath, const bool loadNewerIfNotFound = true);

		/// <summary>
		/// 最初に設定されるべき値のセット
		/// </summary>
		/// <param name="posX">X座標</param>
		/// <param name="posY">Y座標</param>
		/// <param name="width">横幅</param>
		/// <param name="height">縦幅</param>
		void SetInitParams(const float posX, const float posY, const float width, const float height);

		/// <summary>
		/// 描画範囲(レクタングル)をセット
		/// </summary>
		/// <param name="tex_x">切り取りx座標</param>
		/// <param name="tex_y">切り取りy座標</param>
		/// <param name="tex_width">切り取り横幅(px)</param>
		/// <param name="tex_height">	切り取り縦幅(px)</param>
		void SetDrawRectangle(const float tex_x, const float tex_y, const float tex_width, const float tex_height);

		/// <summary>
		/// アンカーポイントをセットする
		/// </summary>
		/// <param name="anchorpoint">アンカーポイント</param>
		void SetAnchorpoint(const Vector2& anchorpoint);

		/// <summary>
		/// 描画のブレンド設定をセット
		/// </summary>
		/// <param name="drawMode">ブレンド設定(SPRITE_BLENDMODE_...)</param>
		void SetBlendMode(int blendMode) {
			//範囲外ならなにもしない
			if (blendMode < 0 || blendMode >= PIPELINE_COUNT) {
				return;
			}
			pipelineIndex = blendMode;
		}
#pragma endregion
	};

	/*
		ComPtr<ID3D12RootSignature> Sprite::spriteRootSignature = nullptr;	//ルートシグネチャ
		ComPtr<ID3D12PipelineState> Sprite::spritePipelineState = nullptr;	//パイプラインステート
		ComPtr<ID3D12Resource> Sprite::texbuff = nullptr;					//テクスチャバッファ
		ComPtr<ID3DBlob> Sprite::vsBlob = nullptr; // 頂点シェーダオブジェクト
		ComPtr<ID3DBlob> Sprite::psBlob = nullptr; // ピクセルシェーダオブジェクト
		ComPtr<ID3DBlob> Sprite::errorBlob = nullptr; // エラーオブジェクト
		ComPtr<ID3D12DescriptorHeap> Sprite::basicDescHeap = nullptr;
		XMMATRIX Sprite::spriteMatProjection;		//射影行列

	*/

}