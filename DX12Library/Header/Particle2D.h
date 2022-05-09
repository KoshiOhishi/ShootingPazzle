#pragma once
#include "ParticleManager.h"

namespace DX12Library {

	class Particle2D :
		public ParticleManager
	{
	public:
		//パーティクル1粒
		struct Particle
		{
			//座標
			Vector2 position = {};

			//速度
			Vector2 velocity = {};

			//加速度
			Vector2 accel = {};

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
		struct VertexPos2D
		{
			Vector3 pos; // xyz座標
			Vector2 scale;	//スケール
		};

		// 定数バッファ用データ構造体
		struct ConstBufferData2D
		{
			XMMATRIX mat;	// ビュープロジェクション行列
			Vector4 color; //色
		};

	private:
		// ルートシグネチャ
		static ComPtr<ID3D12RootSignature> rootsignature;
		// パイプラインステートオブジェクト
		static ComPtr<ID3D12PipelineState> pipelinestate[PIPELINE_COUNT];
		// 射影変換行列
		static DirectX::XMMATRIX matProjection;

	public:
		/// <summary>
		/// 静的初期化
		/// </summary>
		static void StaticInitialize();

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
		void Add(double life, const Vector2& position, const Vector2& velocity,
			const Vector2& accel, float start_scale, float end_scale);
		void Add(double life, const Vector2& position, const Vector2& velocity,
			const Vector2& accel, float start_scale, float end_scale, float middle_scale);

	private:
		/// <summary>
		/// モデル作成
		/// </summary>
		void CreateModel();

	private:
		//パーティクル配列
		std::forward_list<Particle> particles;
		// 頂点データ配列
		VertexPos2D vertices[VERTEX_COUNT];
	};
}