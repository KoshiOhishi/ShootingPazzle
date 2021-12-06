#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <fbxsdk.h>
#include "FbxLoader.h"

struct Node
{
	//名前
	std::string name;
	//ローカルスケール
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	//ローカル回転角
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	//ローカル移動
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	//ローカル変形行列
	DirectX::XMMATRIX transform;
	//グローバル変形行列
	DirectX::XMMATRIX globalTransform;
	//親ノード
	Node* parent = nullptr;
};

class FbxModel
{
public: //サブクラス

	//ボーン構造体
	struct Bone
	{
		//名前
		std::string name;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitialPose;
		//クラスター(FBX側のボーン情報)
		FbxCluster* fbxCluster;
		//コンストラクタ
		Bone(const std::string name) {
			this->name = name;
		}
	};

	//マテリアル構造体
	struct Material
	{
		//アルベド
		DirectX::XMFLOAT3 baseColor = { 1,1,1 };
		//金属度(0 or 1)
		float metalness = 0.0f;
		//鏡面反射度(0 ～ 1)
		float specular = 0.5f;
		//粗さ
		float roughness = 0.0f;
	};

private: //エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	//std::を省略
	using string = std::string;
	template<class T> using vector = std::vector<T>;

public: //定数
	static const int MAX_BONE_INDICES = 4;

public: //サブクラス
	//頂点データ構造体
	struct VertexPosNormalUVSkin
	{
		DirectX::XMFLOAT3 pos;	//xyz座標
		DirectX::XMFLOAT3 normal;	//法線ベクトル
		DirectX::XMFLOAT2 uv;	//uv座標
		UINT boneIndex[MAX_BONE_INDICES];	//ボーン　番号
		float boneWeight[MAX_BONE_INDICES];	//ボーン　重み
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;	//アンビエント係数
		float pad1;			//パディング
		XMFLOAT3 diffuse;	//ディフューズ係数
		float pad2;			//パディング
		XMFLOAT3 specular;	//スペキュラー係数
		float alpha;		//アルファ
	};

	//定数バッファ用データ構造体（マテリアル）
	struct ConstBufferDataMaterial
	{
		//アルベド
		DirectX::XMFLOAT3 baseColor;
		//金属度
		float metalness;
		//鏡面反射度
		float specular;
		//粗さ
		float roughness;
		//パディング(16Byte境界)
		float pad[2];
	};

public:
	//フレンドクラス
	friend class FbxLoader;

public: //メンバ関数
	~FbxModel();

	//バッファ生成
	void CreateBuffers(ID3D12Device* device);

	//描画
	void Draw();

	//マテリアルパラメータ転送
	void TransferMaterial();

	//モデル読み込み(fbx)
	void CreateFromFBX(const string& modelName);

public: //Getter

	//モデルの変形行列取得
	const XMMATRIX& GetModelTransForm() { return meshNode->globalTransform; }

	//ボーン配列取得
	std::vector<Bone>& GetBones() { return bones; }

	//シーン取得
	FbxScene* GetFbxScene() { return fbxScene; }

	const DirectX::XMFLOAT3& GetBaseColor() { return material.baseColor; }
	float GetMetalness() { return material.metalness; }
	float GetSpecular() { return material.specular; }
	float GetRoughness() { return material.roughness; }

//Setter
	void SetBaseColor(const DirectX::XMFLOAT3& _baseColor) { material.baseColor = _baseColor; }
	void SetMetalness(float _metalness) { material.metalness = _metalness; }
	void SetSpecular(float _specular) { material.specular = _specular; }
	void SetRoughness(float _roughness) { material.roughness = _roughness; }

private: //メンバ変数

	//モデル名
	std::string name;
	//ノード配列
	std::vector<Node> nodes;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//ボーン配列
	std::vector<Bone> bones;

	//FBXシーン
	FbxScene* fbxScene = nullptr;

	//頂点データ配列
	std::vector<VertexPosNormalUVSkin> vertices;
	//頂点インデックス配列
	std::vector<unsigned short> indices;

	//アンビエント係数
	DirectX::XMFLOAT3 ambient = { 1,1,1 };
	//ディフューズ係数
	DirectX::XMFLOAT3 diffuse = { 1,1,1 };
	//スぺキュラ係数
	DirectX::XMFLOAT3 specular = { 0, 0, 0 };
	//アルファ
	float alpha = 1;
	//テクスチャメタデータ
	DirectX::TexMetadata metadata = {};
	//スクラッチイメージ
	DirectX::ScratchImage scratchImg = {};

	//UE4マテリアル
	Material material;

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//インデｋックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;
	//定数バッファ（マテリアル）
	ComPtr<ID3D12Resource> constBuffMaterial;
};

