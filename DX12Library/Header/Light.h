#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "Vector3.h"
#include "Camera.h"

/// <summary>
/// ライト
/// </summary>
class Light
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector3 lightDir;	//ライトへの方向を表すベクトル
		float pad; //パディング
		Vector3 lightColor;	//ライトの色
	};


private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ライト位置
	Vector3 lightPos = { 0,0,0 };
	//ライト光線方向（単位ベクトル）
	Vector3 lightDir = Vector3(1,-1,1).Normalize();
	//ライト色
	Vector3 lightColor = { 1,1,1 };
	//ライト注視点
	Vector3 lighttarget = { 0,0,0 };
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
	/// ライトの位置算出
	/// </summary>
	/// <param name="camera"></param>
	void CalcLightPos(float distance);

	/// <summary>
	/// ライトの座標をセット
	/// (よほどじゃない限りCalcLightPosを使用してください)
	/// </summary>
	/// <param name="lightpos"></param>
	void SetLightPos(const Vector3& lightPos) { this->lightPos = lightPos; }

	/// <summary>
	/// ライトの照点距離セット
	/// </summary>
	/// <param name="lightdistance"></param>
	void SetLightTarget(const Vector3& lighttarget) { this->lighttarget = lighttarget; }

	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="lightdir">ライト方向</param>
	void SetLightDir(const Vector3& lightDir);

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

	/// <summary>
	/// ライトの方向取得
	/// </summary>
	/// <returns></returns>
	const Vector3& GetLightDir()const { return lightDir; }

	/// <summary>
	/// ライトの位置取得
	/// </summary>
	/// <returns></returns>
	const Vector3& GetLightPos()const { return lightPos; }

	/// <summary>
	/// ライトの照点距離取得
	/// </summary>
	/// <returns></returns>
	const Vector3& GetLightTarget()const { return lighttarget; }
};

