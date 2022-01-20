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

enum ParticleBillboardType {
	BILLBOARD_TYPE_NONE,
	BILLBOARD_TYPE_XYZ,
	BILLBOARD_TYPE_Y
};

enum ParticleBlendMode {
	PARTICLE_BLENDMODE_NORMAL,	//通常描画
	PARTICLE_BLENDMODE_ADD,		//加算合成
	PARTICLE_BLENDMODE_SUB,		//減算合成
	PARTICLE_BLENDMODE_MUL,		//乗算合成
	PARTICLE_BLENDMODE_SCREEN,	//スクリーン合成
	PARTICLE_BLENDMODE_REVERSE	//反転
};

/// <summary>
/// 3Dオブジェクト
/// </summary>
class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPos
	{
		Vector3 pos; // xyz座標
		float scale;	//スケール
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMMATRIX mat;	// ビュープロジェクション行列
		XMMATRIX matBillboard;	//ビルボード行列
	};

	//パーティクル1粒
	struct Particle
	{
		//座標
		Vector3 position = {};

		//速度
		Vector3 velocity = {};

		//加速度
		Vector3 accel = {};

		//生存時間
		Timer life;

		//スケール
		float scale = 1.0f;
		//初期値
		float s_scale = 1.0f;
		//最終値
		float e_scale = 0.0f;
		//中間値
		float m_scale = 0.5f;
	};

private: // 定数
	static const int PIPELINE_COUNT = 6;
	static const int PATICLE_SRV_COUNT;
	static const int DIVISION = 50;					// 分割数
	static const int PLANE_COUNT = DIVISION * 2 + DIVISION * 2;		// 面の数
	static const int VERTEX_COUNT = 4096;		// 頂点数
	//static const int indexCount = 3 * 2;	//インデックス数

private: // 静的メンバ変数
	static std::vector<ParticleManager*> drawList;
	static std::vector<ParticleManager*> drawListFG;

	// デバイス
	static ID3D12Device* device;
	//カメラ
	static Camera* camera;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate[PIPELINE_COUNT];
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	// 頂点バッファ
	static ComPtr<ID3D12Resource> vertBuff;
	//// インデックスバッファ
	//static ComPtr<ID3D12Resource> indexBuff;
	// テクスチャバッファ
	static ComPtr<ID3D12Resource> texbuff[];
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vbView;
	//// インデックスバッファビュー
	//static D3D12_INDEX_BUFFER_VIEW ibView;
	// 頂点データ配列
	static VertexPos vertices[VERTEX_COUNT];
	//// 頂点インデックス配列
	//static unsigned short indices[indexCount];
	//ロードされたテクスチャ数
	static UINT loadedTextureCount;
	//ロードされたテクスチャのファイル名と番号を紐づけるリスト
	static std::unordered_map<std::wstring, UINT> loadedTextureList;

public:// 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// カメラセット
	/// </summary>
	/// <param name="camera"></param>
	static void SetCamera(Camera* camera) { ParticleManager::camera = camera; }

	/// <summary>
	/// コンテナに溜めたパーティクルを全て描画
	/// </summary>
	static void DrawAll();

	/// <summary>
	/// コンテナに溜めたパーティクルを全て描画
	/// </summary>
	static void DrawAllFG();

private:
	/// <summary>
	/// 描画前処理
	/// </summary>
	static void BeginDraw();

	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	static void CreateDescriptorHeap();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	static void CreateGraphicsPipeline();

	/// <summary>
	/// モデル作成
	/// </summary>
	static void CreateModel();


public: // メンバ関数
	bool Initialize();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画
	/// </summary>
	void DrawFG();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture(const std::wstring& filename);

	/// <summary>
	/// ビルボードタイプの設定
	/// </summary>
	/// <param name="billboardType">ビルボードタイプ 0...無し 1...全方向 2...Y軸</param>
	void SetBillboardType(int billboardType) { this->billboardType = billboardType; }

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
	/// パーティクルの追加
	/// </summary>
	/// <param name="life">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="start_scale">開始時スケール値</param>
	/// <param name="end_scale">終了時スケール値</param>
	void Add(double life, const Vector3& position, const Vector3& velocity, 
		const Vector3& accel, float start_scale, float end_scale);
	void Add(double life, const Vector3& position, const Vector3& velocity,
		const Vector3& accel, float start_scale, float end_scale, float middle_scale);

private: // メンバ変数
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	// ローカルスケール
	Vector3 scale = { 1,1,1 };
	//ビルボードタイプ
	int billboardType = BILLBOARD_TYPE_XYZ;
	//パーティクル配列
	std::forward_list<Particle> particles;
	//テクスチャ番号
	UINT texNumber;
	//使用するパイプラインのインデックス
	int pipelineIndex = 0;

private:
	void DrawPrivate();
};

