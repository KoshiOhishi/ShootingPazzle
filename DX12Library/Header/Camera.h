#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class Camera
{
protected: //エイリアス
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
	using string = std::string;

public: //サブクラス
	enum Axis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	static const float CAMERA_NEAR;
	static const float CAMERA_FAR;
	static const float CAMERA_VIEWING_ANGLE;

protected: //メンバ変数

// 射影変換行列
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();

	// ビュー変換行列
	XMMATRIX matView = DirectX::XMMatrixIdentity();

	// 回転行列
	XMMATRIX matRotation = DirectX::XMMatrixIdentity();

	//ビルボード行列
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();

	//Y軸回りビルボード行列
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();

	// 視点座標
	Vector3 eye = { 0,0,-100 };

	// 注視点座標
	Vector3 target = { 0, 0, 0 };

	// 上方向ベクトル
	Vector3 up = { 0, 1, 0 };

	// カメラ回転角
	Vector3 rotation = { 0,0,0 };

	//ダーティフラグ
	bool dirty = false;

public: //メンバ関数

	//初期化
	virtual void Initialize();

	//更新
	virtual void Update();

	/// <summary>
	/// カメラを移動させる処理
	/// </summary>
	/// <param name="vector">移動量ベクトル</param>
	/// <param name="isAutoDirAxisX">カメラが向いている方向を基準に移動するかどうか（X軸基準）</param>
	/// <param name="isAutoDirAxisY">カメラが向いている方向を基準に移動するかどうか（Y軸基準）</param>
	/// <param name="isAutoDirAxisZ">カメラが向いている方向を基準に移動するかどうか（Z軸基準）</param>
	void MoveCamera(const Vector3& vector, bool isAutoDirAxisX = false, bool isAutoDirAxisY = true, bool isAutoDirAxisZ = false);

	/// <summary>
	/// カメラを移動させる処理
	/// </summary>
	/// <param name="x">移動量x</param>
	/// <param name="y">移動量y</param>
	/// <param name="z">移動量z</param>
	/// <param name="isAutoDirAxisX">カメラが向いている方向を基準に移動するかどうか（X軸基準）</param>
	/// <param name="isAutoDirAxisY">カメラが向いている方向を基準に移動するかどうか（Y軸基準）</param>
	/// <param name="isAutoDirAxisZ">カメラが向いている方向を基準に移動するかどうか（Z軸基準）</param>
	void MoveCamera(const float x, const float y, const float z, bool isAutoDirAxisX = false, bool isAutoDirAxisY = true, bool isAutoDirAxisZ = false);

	/// <summary>
	/// カメラを今の角度から指定量回転
	/// </summary>
	/// <param name="axis">回転軸</param>
	/// <param name="digrees">回転量</param>
	void RotateCamera(const Vector3& axis, float digrees);

	/// <summary>
	/// ビュー行列の更新処理
	/// </summary>
	void UpdateViewMatrix();

	//Getter
public:
	//ビュープロジェクション行列取得
	const XMMATRIX GetViewProjection()const;

	//ビルボード行列取得
	const XMMATRIX& GetBillboard()const { return matBillboard; }

	//Y軸回りビルボード行列取得
	const XMMATRIX& GetBillboardY()const { return matBillboardY; }

	/// <summary>
	/// 視点座標取得
	/// </summary>
	/// <returns>視点座標</returns>
	const Vector3& GetPosition()const { return eye; }

	/// <summary>
	/// 注視点座標取得
	/// </summary>
	/// <returns>注視点座標</returns>
	const Vector3 GetTarget()const { return eye + GetCameraDir() * GetDistance(); }

	/// <summary>
	/// 上方向ベクトル取得
	/// </summary>
	/// <returns>上方向ベクトル</returns>
	const Vector3 GetUpVec()const;

	/// <summary>
	/// 視点座標から注視点座標の距離を取得
	/// </summary>
	/// <returns>視点座標から注視点座標の距離</returns>
	const float GetDistance()const;

	/// <summary>
	/// カメラの回転角を取得
	/// </summary>
	/// <returns>回転角(x = pitch, y = yaw, z = roll)</returns>
	const Vector3& GetRotation()const { return rotation; }

	/// <summary>
	/// カメラの回転行列を取得
	/// </summary>
	/// <returns>回転行列</returns>
	const XMMATRIX& GetRotMatrix()const { return matRotation; }

	/// <summary>
	/// ビュー行列を取得
	/// </summary>
	/// <returns>ビュー行列を</returns>
	const XMMATRIX GetViewMatrix()const { return matView; }

	/// <summary>
	/// 射影変換行列を取得
	/// </summary>
	/// <returns>射影変換行列</returns>
	const XMMATRIX& GetProjectionMatrix()const { return matProjection; }

	/// <summary>
	/// カメラの向いている方向を取得
	/// </summary>
	/// <returns></returns>
	const Vector3 GetCameraDir()const;


//Setter
public:
	//視点座標をセット
	void SetEye(const Vector3& eye);
	void SetEye(const float x, const float y, const float z);
	//注視点座標をセット
	void SetTarget(const Vector3& target);
	void SetTarget(const float x, const float y, const float z);
	//上方向ベクトルをセット
	void SetUpVec(const Vector3& up);
	void SetUpVec(const float x, const float y, const float z);

	/// <summary>
	/// 視点座標、注視点座標、上方向ベクトルをセット (初期化用)
	/// </summary>
	/// <param name="eye">視点座標</param>
	/// <param name="target">注視点座標</param>
	/// <param name="up">上ベクトル</param>
	void SetEyeTargetUpVec(const Vector3& eye, const Vector3& target, const Vector3& up);

	/// <summary>
	/// 視点座標、注視点座標、上方向ベクトルをセット (初期化用)
	/// </summary>
	/// <param name="eyeX">視点X座標</param>
	/// <param name="eyeY">視点Y座標</param>
	/// <param name="eyeZ">視点Z座標</param>
	/// <param name="targetX">注視点X座標</param>
	/// <param name="targetY">注視点Y座標</param>
	/// <param name="targetZ">注視点Z座標</param>
	/// <param name="upX">上ベクトルX</param>
	/// <param name="upY">上ベクトルY</param>
	/// <param name="upZ">上ベクトルZ</param>
	void SetEyeTargetUpVec(
		const float eyeX, const float eyeY, const float eyeZ,
		const float targetX, const float targetY, const float targetZ,
		const float upX, const float upY, const float upZ);

	/// <summary>
	/// 視点座標、注視点座標までの距離をまとめてセット
	/// </summary>
	/// <param name="pos">視点座標</param>
	/// <param name="distance">視点座標から注視点座標の距離</param>
	void SetPositionAndDistance(const Vector3& pos, const float distance);

	/// <summary>
	/// 視点座標、注視点座標までの距離をまとめてセット
	/// </summary>
	/// <param name="posX">視点X座標</param>
	/// <param name="posY">視点Y座標</param>
	/// <param name="posZ">視点Z座標</param>
	/// <param name="distance">視点座標から注視点座標の距離</param>
	void SetPositionAndDistance(const float posX, const float posY, const float posZ, const float distance);

	/// <summary>
	/// 視点座標をセット
	/// </summary>
	/// <param name="pos">視点座標</param>
	void SetPosition(const Vector3& pos);

	/// <summary>
	/// 視点座標をセット
	/// </summary>
	/// <param name="x">視点X座標</param>
	/// <param name="y">視点Y座標</param>
	/// <param name="z">視点Z座標</param>
	void SetPosition(const float x, const float y, const float z);

	/// <summary>
	/// 視点座標から注視点座標の距離をセット
	/// </summary>
	/// <param name="distance">視点座標から注視点座標の距離</param>
	void SetDistance(const float distance);

	/// <summary>
	/// カメラの回転角をセット
	/// </summary>
	/// <param name="vector">回転量(度)</param>
	void SetRotation(const Vector3& rotation);

	/// <summary>
	/// カメラの回転角をセット
	/// </summary>
	/// <param name="pitch">X軸回転角(度)</param>
	/// <param name="yaw">Y軸回転角(度)</param>
	/// <param name="roll">Z軸回転角(度)</param>
	void SetRotation(const float pitch, const float yaw, const float roll);

private:
	/// <summary>
	/// 視点座標を指定分加算する
	/// </summary>
	/// <param name="eyeX">X座標</param>
	/// <param name="eyeY">Y座標</param>
	/// <param name="eyeZ">Z座標</param>
	void AddEye(const float eyeX, const float eyeY, const float eyeZ);

	/// <summary>
	/// 注視点座標を指定分加算する
	/// </summary>
	/// <param name="targetX">X座標</param>
	/// <param name="targetY">Y座標</param>
	/// <param name="targetZ">Z座標</param>
	void AddTarget(const float targetX, const float targetY, const float targetZ);

	/// <summary>
	/// 上方向ベクトルを指定分加算する
	/// </summary>
	/// <param name="upX">x成分</param>
	/// <param name="upY">y成分</param>
	/// <param name="upZ">z成分</param>
	void AddUp(const float upX, const float upY, const float upZ);
};

