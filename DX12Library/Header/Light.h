#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "Vector3.h"

/// <summary>
/// ライト
/// </summary>
class Light
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR lightv;	//ライトへの方向を表すベクトル
		Vector3 lightcolor;	//ライトの色
	};


private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ライト光線方向（単位ベクトル）
	XMVECTOR lightdir = {1,0,0,0};
	//ライト色
	Vector3 lightcolor = { 1,1,1 };
	//ダーティフラグ
	bool dirty = false;

public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(UINT rootParameterIndex);

	/// <summary>
	/// 定数バッファ転送
	/// </summary>
	void TransferConstBuffer();

	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="lightdir">ライト方向</param>
	void SetLightDir(const XMVECTOR& lightdir);

	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="dirX">ライト方向X</param>
	/// <param name="dirY">ライト方向Y</param>
	/// <param name="dirZ">ライト方向Z</param>
	void SetLightDir(const float dirX, const float dirY, const float dirZ);

	/// <summary>
	/// ライト色をセット
	/// </summary>
	/// <param name="lightcolor">ライト色</param>
	void SetLightColor(const Vector3& lightcolor);

	/// <summary>
	/// ライト色をセット (0～1指定バージョン)
	/// </summary>
	/// <param name="R">赤成分</param>
	/// <param name="G">緑成分</param>
	/// <param name="B">青成分</param>
	void SetLightColor(const float R, const float G, const float B);

	/// <summary>
	/// ライト色をセット (0～255指定バージョン)
	/// </summary>
	/// <param name="R">赤成分</param>
	/// <param name="G">緑成分</param>
	/// <param name="B">青成分</param>
	void SetLightColorAs0To255(const float R, const float G, const float B);
};

