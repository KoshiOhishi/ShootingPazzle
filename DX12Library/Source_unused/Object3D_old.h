#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <vector>
#include "ObjModel.h"
#include "Light.h"
#include "Camera.h"
#pragma comment(lib, "d3d12.lib")

class Object3D_OLD
{
private: //エイリアス

	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMVECTOR = DirectX::XMVECTOR;
	using string = std::string;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <typename T>
	using vector = std::vector<T>;
		
//サブクラス
public:

	// 定数バッファ用データ構造体
	struct ConstBufferDataB0 {
		XMFLOAT4 color;	//色(RGBA)
		XMMATRIX viewproj;	//ビュープロジェクション行列
		XMMATRIX world;		//ワールド行列
		XMFLOAT3 cameraPos;	//カメラ座標（ワールド座標）
	};

	enum BillboardType {
		NONE,
		Billboard,
		BillboardY,
	};

//定数
private:


//メンバ変数
protected:


	// スケーリング倍率
	XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};

	// 回転角
	XMFLOAT3 rotation = { 0,0,0 };

	// 座標
	XMFLOAT3 position = { 0,0,0 };

	// ワールド座標
	XMMATRIX matWorld{};

	//モデル
	ObjModel model;

	//ワイヤフレーム表示
	bool isWireFlame = false;

	//色
	XMFLOAT4 color = { 1,1,1,1 };

	//ビルボードタイプ
	BillboardType billboardType = BillboardType::NONE;

	//ライト効果の有無
	bool isLight = true;

	//親オブジェクトのポインタ
	Object3D_OLD* parent = nullptr;

#pragma region 初期化回りの変数


	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffB0;


#pragma endregion

//静的メンバ変数
private:

	//カメラ
	static Camera* camera;

	// デスクリプタヒープ
	static ComPtr <ID3D12DescriptorHeap> basicDescHeap;

	// OBJ用ルートシグネチャ
	static ComPtr <ID3D12RootSignature> rootsignatureOBJ;

	// OBJ用パイプラインステート
	static ComPtr <ID3D12PipelineState> pipelinestateOBJ;

	// 数式用ルートシグネチャ
	static ComPtr <ID3D12RootSignature> rootsignatureMath;

	// 数式オブジェクト用パイプラインステート
	static ComPtr <ID3D12PipelineState> pipelinestateMath;

	// 影無しオブジェクト用ルートシグネチャ
	static ComPtr <ID3D12RootSignature> rootsignatureNoShade;

	// 影無しオブジェクト用パイプラインステート
	static ComPtr <ID3D12PipelineState> pipelinestateNoShade;

	// ワイヤフレーム用パイプラインステート
	static ComPtr <ID3D12PipelineState> pipelinestateWire;

	//ライト
	static Light* light;

//メンバ関数

public:

	/// <summary>
	/// 初期化処理 ※LoadOBJの後に記述！
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// モデルをセットする
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(ObjModel model);

#pragma region 便利関数
//メンバ関数
public:
	/// <summary>
	/// スケールを指定分加算する
	/// </summary>
	/// <param name="scaleX">スケールX</param>
	/// <param name="scaleY">スケールY</param>
	/// <param name="scaleZ">スケールZ</param>
	void AddScale(const float scaleX, const float scaleY, const float scaleZ)
	{
		this->scale.x += scaleX; this->scale.y += scaleY; this->scale.z += scaleZ;
	}

	/// <summary>
	/// 回転角を指定分加算する
	/// </summary>
	/// <param name="rotationX">回転角X</param>
	/// <param name="rotationY">回転角Y</param>
	/// <param name="rotationZ">回転角Z</param>
	void AddRotation(const float rotationX, const float rotationY, const float rotationZ)
	{
		this->rotation.x += rotationX; this->rotation.y += rotationY; this->rotation.z += rotationZ;
	}

	/// <summary>
	/// 座標を指定分加算する
	/// </summary>
	/// <param name="positionX">X座標</param>
	/// <param name="positionY">Y座標</param>
	/// <param name="positionZ">Z座標</param>
	void AddPosition(const float positionX, const float positionY, const float positionZ)
	{
		this->position.x += positionX; this->position.y += positionY; this->position.z += positionZ;
	}

#pragma endregion


//静的メンバ関数
public:

	/// <summary>
	/// ライトのセット
	/// </summary>
	/// <param name="light">ライト</param>
	static void SetLight(Light* light) { Object3D_OLD::light = light; }
		
	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	static void SetCamera(Camera* camera) { Object3D_OLD::camera = camera; }

//静的メンバ関数
public:
	
	/// <summary>
	/// Object3D自体の初期化。最初に書く。
	/// </summary>
	static void FirstInit();

	static void InitPipelineOBJ();

	static void InitPipelineMath();

	static void InitPipelineNoShade();

#pragma region Getter
//メンバ関数
public:
	/// <summary>
	/// スケールの取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetScale() { return scale; }

	/// <summary>
	/// 回転角の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetRotation() { return rotation; }


	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetPosition() { return position; }

	/// <summary>
	/// ワイヤフレーム描画フラグを取得
	/// </summary>
	/// <returns>ワイヤフレームフラグ</returns>
	const bool& GetIsWireFlame() { return isWireFlame; }

	/// <summary>
	/// 色(RGBA)を取得する
	/// </summary>
	/// <returns>色(RGBA)</returns>
	const XMFLOAT4& GetColor() { return color; }

	/// <summary>
	/// 色(RGBA)を0～255の数値で取得する
	/// </summary>
	/// <returns>色(RGBA)、0～255の範囲</returns>
	const XMFLOAT4& GetColorAs0To255(){
		XMFLOAT4 c = { color.x * 255,color.y * 255 ,color.z * 255 ,color.w * 255 };
		return c;
	}

	/// <summary>
	/// ビルボードの種類を返す
	/// </summary>
	/// <returns>ビルボードの種類</returns>
	const BillboardType GetBillboardType() { return billboardType; }

	/// <summary>
	/// ライトの有無を返す
	/// </summary>
	/// <returns>ライトの有無</returns>
	const bool GetIsLight() { return isLight; }

private:
	/// <summary>
	/// 親子構造用ワールド行列ゲッター
	/// </summary>
	const XMMATRIX GetMatWorld() { return matWorld; }

//静的メンバ関数
public:

	/// <summary>
	/// デスクリプタヒープのポインタを取得
	/// </summary>
	/// <returns>デスクリプタヒープのポインタ</returns>
	static ID3D12DescriptorHeap* GetDescHeap();

#pragma endregion

#pragma region Setter
//メンバ関数
public:
	/// <summary>
	/// スケールをセット
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const XMFLOAT3 scale) { this->scale = scale; }

	/// <summary>
	/// スケールをセット
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetScale(const float x, const float y, const float z) { this->scale = {x,y,z}; }

	/// <summary>
	/// 回転角をセット
	/// </summary>
	/// <param name="rotation">回転角</param>
	void SetRotation(const XMFLOAT3 rotation) { this->rotation = rotation; }

	/// <summary>
	/// 回転角をセット
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetRotation(const float x, const float y, const float z) { this->rotation = { x,y,z }; }


	/// <summary>
	/// 座標をセット
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const XMFLOAT3 position) { this->position = position; }

	/// <summary>
	/// 座標をセット
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetPosition(const float x, const float y, const float z) { this->position = { x,y,z }; }

	/// <summary>
	/// ワイヤフレームフラグをセット
	/// </summary>
	void SetIsWireFlame(const bool isWireFlame) { this->isWireFlame = isWireFlame; }

	/// <summary>
	/// 色(RGBA)をセット
	/// </summary>
	/// <param name="R">赤</param>
	/// <param name="G">緑</param>
	/// <param name="B">青</param>
	/// <param name="A">アルファ値</param>
	void SetColor(float R, float G, float B, float A) {
		XMFLOAT4 c = { R,G,B,A };
		SetColor(c);
	}

	/// <summary>
	/// 色(RGBA)をセット
	/// </summary>
	/// <param name="color">色(RGBA)</param>
	void SetColor(XMFLOAT4 color) { this->color = color; }

	/// <summary>
	/// 色(RGBA)を0～255の範囲でセット
	/// </summary>
	/// <param name="R">赤</param>
	/// <param name="G">緑</param>
	/// <param name="B">青</param>
	/// <param name="A">アルファ値</param>
	void SetColorAs0To255(float R, float G, float B, float A = 255)
	{
		R = R / 255;
		G = G / 255;
		B = B / 255;
		A = A / 255;
		XMFLOAT4 c = { R,G,B,A };
		SetColor(c);
	}

	/// <summary>
	/// 色(RGBA)を0～255の範囲でセット
	/// </summary>
	/// <param name="color">色(RGBA)</param>
	void SetColorAs0To255(XMFLOAT4 color)
	{
		XMFLOAT4 c = { color.x / 255, color.y / 255, color.z / 255, color.w / 255 };
		SetColor(c);
	}

	/// <summary>
	/// ビルボードの種類をセットする 引数はObject3D::BillboardTypeで渡す
	/// </summary>
	/// <param name="billboardType">ビルボードの種類</param>
	void SetBillboardType(BillboardType billboardType) { this->billboardType = billboardType; }

	/// <summary>
	/// ライトの有無フラグをセット
	/// </summary>
	/// <param name="isLight">ライトの有無</param>
	void SetIsLight(bool isLight) { this->isLight = isLight; }

	/// <summary>
	/// 親オブジェクトをセット
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(Object3D_OLD* parent) { this->parent = parent; }

//静的メンバ関数
public:

#pragma endregion
};
