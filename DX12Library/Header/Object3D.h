#pragma once
#include "FbxLoader.h"
#include "FbxModel.h"
#include "ObjModel.h"
#include "Camera.h"
#include "Light.h"
#include "DX12Util.h"
#include "CollisionInfo.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

class BaseCollider;

//オブジェクトの種類
enum ObjectType
{
	OBJECTTYPE_OBJ,
	OBJECTTYPE_FBX,
	OBJECTTYPE_INSTANCING_OBJ,
	OBJECTTYPE_INSTANCING_FBX,
};

//ビルボードタイプ
enum BillboardType {
	NONE,
	Billboard,
	BillboardY,
};

class Object3D
{
protected: //エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;
	//std::を省略
	using string = std::string;

	using PipelineData = DX12Util::PipelineData;

public: //定数
	//ボーンの最大数
	static const int MAX_BONES = 32;

public: //サブクラス

	//インスタンシング描画用構造体(座標変換行列用)
	struct InstanceData
	{
		Vector4 color;		//色
		DirectX::XMMATRIX world;		//ワールド行列
	};

	struct ConstBufferDataShare
	{
		DirectX::XMMATRIX viewproj;	//ビュープロジェクション行列
		DirectX::XMMATRIX lightCamera;
		UINT isDrawShadowToMyself;
		Vector3 cameraPos;	//カメラ座標(ワールド座標)
	};

	//定数バッファ用データ構造体(座標変換)
	struct ConstBufferDataTransform
	{
		InstanceData data;
	};

	//定数バッファ用データ構造体(スキニング)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

	//定数バッファ用データ構造体(シャドウマップ)
	struct ConstBufferDataShadow
	{
		XMMATRIX lightCamera;
		XMMATRIX world;
	};

public: //静的メンバ関数

	/// <summary>
	/// デスクリプタヒープ生成
	/// </summary>
	static void StaticInitialize();
	
	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	/// <param name="objectType">生成するパイプラインが使用されるオブジェクトの種類</param>
	/// <param name="pipelineData">パイプライン設定変数</param>
	static void CreateGraphicsPipeline(int objectType, PipelineData& pipelineData);

	/// <summary>
	/// OBJシャドウ用パイプライン生成
	/// </summary>
	static void CreateShadowObjGraphicsPipeline();

	/// <summary>
	/// FBXシャドウ用パイプライン生成
	/// </summary>
	static void CreateShadowFbxGraphicsPipeline();

	//getter
	static ID3D12DescriptorHeap* GetDescHeap() { return Object3D::descHeapSRV.Get(); }

	//setter
	static void SetDevice(ID3D12Device* device) { Object3D::device = device; }
	static void SetCamera(Camera* camera) { Object3D::camera = camera; }
	static void SetLight(Light* light) { Object3D::light = light; }

protected: //静的メンバ変数
	static std::vector<Object3D*> drawList;

	//デバイス
	static ID3D12Device* device;
	//カメラ
	static Camera* camera;
	//ライト
	static Light* light;
	//SRV用デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//深度テクスチャSRV用ヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeapDepthSRV;
	//FBXルートシグネチャ
	static ComPtr<ID3D12RootSignature> fbxRootsignature;
	//FBXパイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> fbxPipelinestate;
	//OBJシャドウルートシグネチャ
	static ComPtr<ID3D12RootSignature> shadowFbxRootsignature;
	//OBJシャドウパイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> shadowFbxPipelinestate;
	//OBJルートシグネチャ
	static ComPtr<ID3D12RootSignature> objRootsignature;
	//OBJパイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> objPipelinestate;
	//OBJシャドウルートシグネチャ
	static ComPtr<ID3D12RootSignature> shadowObjRootsignature;
	//OBJシャドウパイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> shadowObjPipelinestate;

	static int prevDrawObjectType;

	static int loadCount;

	static XMMATRIX matOrthographicLH;

public:

	static void DrawAll();
	static void WriteDepthTex();

	static int GetLoadCount() { return Object3D::loadCount; }
	static void IncrementLoadCount() { Object3D::loadCount++; }
	static void SetMatrixOrthographicLH(float viewWigth, float viewHeight, float nearZ, float farZ) {
		matOrthographicLH = DirectX::XMMatrixOrthographicLH(viewWigth, viewHeight, nearZ, farZ);
	}

private:
	/// <summary>
	/// 描画
	/// </summary>
	virtual void DrawPrivate();

	/// <summary>
	/// シャドウ描画
	/// </summary>
	virtual void DrawShadow();

	/// <summary>
	/// オブジェクト描画前処理
	/// </summary>
	static void BeginDraw();

	/// <summary>
	/// シャドウ描画前処理
	/// </summary>
	static void BeginDrawShadow();

public: //メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3D() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Object3D();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画リストに登録 (登録された順に最後にまとめて描画する)
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	virtual void OnCollision(const CollisionInfo& info){}

	/// <summary>
	/// アニメーション開始
	/// </summary>
	void PlayAnimation();

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

#pragma region Getter
	//メンバ関数
public:
	/// <summary>
	/// スケールの取得
	/// </summary>
	/// <returns>座標</returns>
	const Vector3& GetScale() const { return scale; }

	/// <summary>
	/// 回転角の取得
	/// </summary>
	/// <returns>座標</returns>
	const Vector3& GetRotation() const { return rotation; }


	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const Vector3& GetPosition() const { return position; }

	/// <summary>
	/// 色(RGBA)を取得する
	/// </summary>
	/// <returns>色(RGBA)</returns>
	const Vector4& GetColor() const { return color; }

	/// <summary>
	/// 色(RGBA)を0～255の数値で取得する
	/// </summary>
	/// <returns>色(RGBA)、0～255の範囲</returns>
	const Vector4 GetColorAs0To255() const {
		Vector4 c = { color.x * 255,color.y * 255 ,color.z * 255 ,color.w * 255 };
		return c;
	}

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	/// <returns>ワールド行列</returns>
	const XMMATRIX& GetMatWorld() const { return matWorld; }

	/// <summary>
	/// objモデルを取得
	/// </summary>
	/// <returns>objモデル</returns>
	inline ObjModel* GetObjModel() const { return objModel; }

	/// <summary>
	/// クラス名取得
	/// </summary>
	/// <returns></returns>
	const char* GetObjectName() const { return name; }

	/// <summary>
	/// コライダー取得
	/// </summary>
	/// <returns></returns>
	const BaseCollider* GetCollider() const { return collider; }

#pragma endregion

#pragma region Setter
	//メンバ関数
public:
	/// <summary>
	/// スケールをセット
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const Vector3& scale) { this->scale = scale; }

	/// <summary>
	/// スケールをセット
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetScale(const float x, const float y, const float z) { this->scale = { x,y,z }; }

	/// <summary>
	/// 回転角をセット
	/// </summary>
	/// <param name="rotation">回転角</param>
	void SetRotation(const Vector3& rotation) { this->rotation = rotation; }

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
	void SetPosition(const Vector3& position) { this->position = position; }

	/// <summary>
	/// 座標をセット
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="z">z</param>
	void SetPosition(const float x, const float y, const float z) { this->position = { x,y,z }; }

	/// <summary>
	/// 色(RGBA)をセット(0～1指定バージョン)
	/// </summary>
	/// <param name="R">赤</param>
	/// <param name="G">緑</param>
	/// <param name="B">青</param>
	/// <param name="A">アルファ値</param>
	void SetColor(float R, float G, float B, float A) {
		SetColor({ R,G,B,A });
	}

	/// <summary>
	/// 色(RGBA)をセット
	/// </summary>
	/// <param name="color">色(RGBA)</param>
	void SetColor(const Vector4& color) { this->color = color; }

	/// <summary>
	/// 色(RGBA)を0～255の範囲でセット
	/// </summary>
	/// <param name="R">赤</param>
	/// <param name="G">緑</param>
	/// <param name="B">青</param>
	/// <param name="A">アルファ値</param>
	void SetColorAs0To255(const float R, const float G, const float B, const float A = 255)
	{
		SetColor({ R / 255,G / 255,B / 255,A / 255 });
	}

	/// <summary>
	/// 色(RGBA)を0～255の範囲でセット
	/// </summary>
	/// <param name="color">色(RGBA)</param>
	void SetColorAs0To255(const Vector4& color)
	{
		SetColor({ color.x / 255, color.y / 255, color.z / 255, color.w / 255 });
	}

	/// <summary>
	/// コライダーのセット
	/// </summary>
	/// <param name="collider">コライダー</param>
	void SetCollider(BaseCollider* collider);

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="fbxModel">FBXモデル</param>
	virtual void SetFbxModel(FbxModel* fbxModel);

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="objModel">OBJモデル</param>
	virtual void SetObjModel(ObjModel* objModel);

	/// <summary>
	/// 自分の影を他のオブジェクトに描画するかセット
	/// </summary>
	/// <param name="flag"></param>
	virtual void SetDrawShadowToOther(bool flag) { isDrawShadowToOther = flag; }

	/// <summary>
	/// 他のオブジェクト影を自分に描画するかセット
	/// </summary>
	/// <param name="flag"></param>
	virtual void SetDrawShadowToMyself(bool flag) { isDrawShadowToMyself = flag; }
#pragma endregion

protected: //メンバ変数
	//定数バッファ(共有データ)
	ComPtr<ID3D12Resource> constBuffShare;

	//定数バッファ(座標変換行列用)
	ComPtr<ID3D12Resource> constBuffTransform;

	//定数バッファ(スキン)
	ComPtr<ID3D12Resource> constBuffSkin;

	//定数バッファ(シャドウ)
	ComPtr<ID3D12Resource> constBuffShadow;

	//ローカルスケール
	Vector3 scale = { 1, 1, 1 };
	//X,Y,Z軸回りのローカル回転角
	Vector3 rotation = { 0, 0, 0 };
	//ローカル座標
	Vector3 position = { 0, 0, 0 };
	//ローカルワールド変換行列
	XMMATRIX matWorld;
	//FBXモデル
	FbxModel* fbxModel = nullptr;
	//OBJモデル
	ObjModel* objModel = nullptr;

	//色
	Vector4 color = { 1,1,1,1 };

	//ビルボードタイプ
	BillboardType billboardType = BillboardType::NONE;

	//1フレームの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメーション終了時間
	FbxTime endTime;
	//現在時間（アニメーション）
	FbxTime currentTime;
	//アニメーション再生中
	bool isPlay = false;

	//オブジェクトタイプ
	int objectType = -1;

	bool isDrawShadowToOther = true;
	bool isDrawShadowToMyself = true;

	//クラス名（デバッグ用）
	const char* name = nullptr;
	//コライダー
	BaseCollider* collider = nullptr;

};

//オブジェクトの更新を一括で行うためのクラス
class Object3DManager
{
private:
	template <typename T>
	using vector = std::vector<T>;

private:
	static vector<Object3D*> pObject3DList;

public:
	static void Initialize();
	static void AddObject(Object3D* p);
	static void UpdateAllObject();
	static void DeleteObject(Object3D* p);
	static void DeleteAllObject();

};