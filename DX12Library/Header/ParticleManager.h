#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include <unordered_map>
#include "Camera.h"
#include "Vector3.h"
#include "Timer.h"

enum ParticleBlendMode {
	PARTICLE_BLENDMODE_NORMAL,	//通常描画
	PARTICLE_BLENDMODE_ADD,		//加算合成
	PARTICLE_BLENDMODE_SUB,		//減算合成
	PARTICLE_BLENDMODE_MUL,		//乗算合成
	PARTICLE_BLENDMODE_SCREEN,	//スクリーン合成
	PARTICLE_BLENDMODE_REVERSE	//反転
};

class ParticleManager
{
protected: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;

protected: // 定数
	static const int PIPELINE_COUNT = 6;
	static const int PATICLE_SRV_COUNT;
	static const int DIVISION = 50;					// 分割数
	static const int PLANE_COUNT = DIVISION * 2 + DIVISION * 2;		// 面の数
	static const int VERTEX_COUNT = 4096;		// 頂点数
	//static const int indexCount = 3 * 2;	//インデックス数

public:
	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	static void StaticInitialize();

protected: // 静的メンバ変数

	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	// テクスチャバッファ
	static ComPtr<ID3D12Resource> texbuff[];
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	//ロードされたテクスチャ数
	static UINT loadedTextureCount;
	//ロードされたテクスチャのファイル名と番号を紐づけるリスト
	static std::unordered_map<std::wstring, UINT> loadedTextureList;

public: // メンバ関数
	virtual void Initialize() = 0;
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	virtual void DrawPrivate() = 0;

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture(const std::wstring& filename);

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

	/// <summary>
	/// 色セット
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vector4& color) { this->color = color; }

	/// <summary>
	/// 色取得
	/// </summary>
	/// <returns></returns>
	const Vector4& GetColor() { return color; }

protected: // メンバ変数
		//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//テクスチャ番号
	UINT texNumber;
	//使用するパイプラインのインデックス
	int pipelineIndex = 0;
	//色
	Vector4 color = { 1,1,1,1 };
};

