#pragma once
#include "ParticleManager.h"

enum ParticleBillboardType {
	BILLBOARD_TYPE_NONE,
	BILLBOARD_TYPE_XYZ,
	BILLBOARD_TYPE_Y
};

class Particle3D :
	public ParticleManager
{
public:
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

	// 頂点データ構造体
	struct VertexPos3D
	{
		Vector3 pos; // xyz座標
		float scale;	//スケール
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData3D
	{
		XMMATRIX mat;	// ビュープロジェクション行列
		XMMATRIX matBillboard;	//ビルボード行列
		Vector4 color; //色
	};

private:
	//カメラ
	static Camera* camera;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate[PIPELINE_COUNT];

public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// カメラセット
	/// </summary>
	/// <param name="camera"></param>
	static void SetCamera(Camera* camera) { Particle3D::camera = camera; }

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void BeginDraw();

private:

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	static void CreateGraphicsPipeline();

public:
	virtual void Initialize() override;
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() override;

	virtual void DrawPrivate() override;

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

	/// <summary>
	/// ビルボードタイプの設定
	/// </summary>
	/// <param name="billboardType">ビルボードタイプ 0...無し 1...全方向 2...Y軸</param>
	void SetBillboardType(int billboardType) { this->billboardType = billboardType; }

private:
	/// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel();

private:
	//ビルボードタイプ
	int billboardType = BILLBOARD_TYPE_XYZ;
	//パーティクル配列
	std::forward_list<Particle> particles;
	// 頂点データ配列
	VertexPos3D vertices[VERTEX_COUNT];
};