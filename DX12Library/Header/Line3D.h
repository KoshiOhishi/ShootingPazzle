#pragma once
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "Vector3.h"
#include "Camera.h"

namespace DX12Library {

	struct LineInfo {
		Vector3 startPos;
		Vector3 endPos;
		Vector4 startColor;
		Vector4 endColor;
	};

	class Line3D
	{
		struct VertexLine3D {
			Vector3 pos;
			Vector4 color;
		};

		struct ConstBuffData {
			Vector4 color;
			DirectX::XMMATRIX matWorld;
		};

	private:
		static const int MAX_DRAW_COUNT = 4096;

	private:
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:
		//デバイス
		static ID3D12Device* device;
		//カメラ
		static Camera* camera;
		//ルートシグネチャ
		static ComPtr<ID3D12RootSignature> rootsignature;
		//パイプラインステートオブジェクト
		static ComPtr<ID3D12PipelineState> pipelinestate;
		//定数バッファ
		static ComPtr<ID3D12Resource> constBuff;
		//頂点バッファ
		static ComPtr<ID3D12Resource> vertBuff;
		//インデックスバッファ
		static ComPtr<ID3D12Resource> indexBuff;

	private:
		static std::vector<LineInfo> lines;

	public:
		/// <summary>
		/// 描画用隠し関数　DrawLineを使用してください
		/// </summary>
		/// <param name="drawInfo"></param>
		static void DrawAll();

		static void SetCamera(Camera* camera) { Line3D::camera = camera; }

	public:
		/// <summary>
		/// プログラム開始時に呼ぶ関数
		/// </summary>
		static void StaticInitialize();

		/// <summary>
		/// 線描画リストを空にする。描画ループ末尾に置く。
		/// </summary>
		static void Reset();

		/// <summary>
		/// 描画前関数
		/// </summary>
		static void BeginDraw();

		/// <summary>
		/// 3D空間に線を描画する
		/// </summary>
		/// <param name="start">始点</param>
		/// <param name="end">終点</param>
		static void DrawLine(const Vector3& start, const Vector3& end);

		/// <summary>
		/// 3D空間に線を描画する
		/// </summary>
		/// <param name="startPos">始点</param>
		/// <param name="endPos">終点</param>
		/// <param name="color">色</param>
		static void DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector4& color);

		/// <summary>
		/// 3D空間に線を描画する
		/// </summary>
		/// <param name="startPos">始点</param>
		/// <param name="endPos">終点</param>
		/// <param name="startColor">始点の色</param>
		/// <param name="endColor">終点の色</param>
		static void DrawLine(const Vector3& startPos, const Vector3& endPos, const Vector4& startColor, const Vector4& endColor);
	};

}