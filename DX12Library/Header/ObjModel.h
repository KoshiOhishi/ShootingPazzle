#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "Vector2.h"
#include "Vector3.h"
#pragma comment(lib, "d3d12.lib")

class ObjModel
{
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
	using string = std::string;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <typename T>
	using vector = std::vector<T>;

//サブクラス
public:
	struct Vertex
	{
		Vector3 pos;	 //xyz座標
		Vector3 normal; //法線ベクトル
		Vector2 uv;	//uv座標
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		Vector3 ambient;	//アンビエント係数
		float pad1;			//パディング
		Vector3 diffuse;	//ディフューズ係数
		float pad2;			//パディング
		Vector3 specular;	//スペキュラー係数
		float alpha;		//アルファ
	};

	//マテリアル
	struct Material
	{
		std::string name;	//マテリアル名
		Vector3 ambient;	//アンビエント影響度
		Vector3 diffuse;	//ディフューズ影響度
		Vector3 specular;	//スペキュラー影響度
		float alpha;		//アルファ
		std::string textureFilename;	//テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.5f, 0.5f, 0.5f };
			diffuse = { 0.5f, 0.5f, 0.5f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

private:
	// 頂点バッファ
	ComPtr < ID3D12Resource> vertBuff = nullptr;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	// インデックスバッファ
	ComPtr <ID3D12Resource> indexBuff = nullptr;

	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//頂点データ
	vector<Vertex> vertices;

	//インデックスデータ
	vector<unsigned short> indices;

	//マテリアル
	Material material;

	// テクスチャバッファ
	ComPtr <ID3D12Resource> texBuff = nullptr;

	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;

	// シェーダリソースビューのハンドル(GPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;

	//OBJから読み込まれたか否か
	bool isLoadFromOBJFile = true;

	//頂点座標スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;

	//テクスチャ番号
	UINT texNumber;

private:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	bool LoadTexture(const std::string& directoryPath, const std::string& filename);
	bool LoadTextureReturnTexSize(const std::string& directoryPath, const std::string& filename, float* texWidth = nullptr, float* texHeight = nullptr);
	void CreateBuffers();

	/// <summary>
	/// エッジ平滑化データの追加
	/// </summary>
	/// <param name="indexPosition">座標インデックス</param>
	/// <param name="indexVertex">頂点インデックス</param>
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	/// <summary>
	/// 平滑化された頂点法線の計算
	/// </summary>
	void CalculateSmoothedVertexNormals();

//メンバ関数
public:

	void Draw(int instancingCount = 1, bool isShadow = false);

	/// <summary>
	/// 頂点バッファ更新
	/// </summary>
	void UpdateVertBuff();

	/// <summary>
	/// OBJファイルからモデルを作成する（フォルダ名と中身の名前は同一のものにしてください。）
	/// </summary>
	/// <param name="modelname">objファイルを格納してるフォルダ名</param>
	/// <param name="smoothing">エッジ平滑化フラグ</param>
	void CreateFromOBJ(const std::string& modelPath, bool smoothing = false);

	/// <summary>
	/// 1頂点を指定してUV値セット
	/// </summary>
	/// <param name="vertexNum"></param>
	/// <param name="adjustUV"></param>
	void SetVertexUV(int vertexNum, const Vector2& uv);

	/// <summary>
	/// OBJファイルから作られたモデルであるかどうか返す
	/// </summary>
	/// <returns></returns>
	bool GetIsOBJ() { return isLoadFromOBJFile; }

	//以下ポリゴン作成
	/// <summary>
	/// 四角形ポリゴンを作成。中心基準。
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="depth">奥行</param>
	/// <param name="ambient">アンビエント(環境光)</param>
	/// <param name="diffuse">ディフューズ(拡散反射光)</param>
	/// <param name="specular">スペキュラー(鏡面反射光)</param>
	void CreateBox(float width, float height, float depth, bool smoothing = false, const Vector3& ambient = {0.5f,0.5f,0.5f}, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// 球ポリゴンを作成
	/// </summary>
	/// <param name="vertexX">軸の分割数 (3以上)</param>
	/// <param name="vertexY">高さの分割数 (3以上)</param>
	/// <param name="radius">半径</param>
	/// <param name="ambient">アンビエント(環境光)</param>
	/// <param name="diffuse">ディフューズ(拡散反射光)</param>
	/// <param name="specular">スペキュラー(鏡面反射光)</param>
	void CreateSphere(int vertexX, int vertexY, float radius, bool smoothing = false, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// 円柱ポリゴンを作成
	/// </summary>
	/// <param name="vertex">底面の頂点数</param>
	/// <param name="radius">半径</param>
	/// <param name="height">高さ</param>
	/// <param name="ambient">アンビエント(環境光)</param>
	/// <param name="diffuse">ディフューズ(拡散反射光)</param>
	/// <param name="specular">スペキュラー(鏡面反射光)</param>
	void CreatePoll(int vertex, float radius, float height, bool smoothing = false, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// 三角形板ポリゴンを作成
	/// </summary>
	/// <param name="p1">頂点1</param>
	/// <param name="p2">頂点2</param>
	/// <param name="p3">頂点3</param>
	/// <param name="ambient">アンビエント(環境光)</param>
	/// <param name="diffuse">ディフューズ(拡散反射光)</param>
	/// <param name="specular">スペキュラー(鏡面反射光)</param>
	void CreateTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// 四角形板ポリゴンを作成
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="ambient">アンビエント(環境光)</param>
	/// <param name="diffuse">ディフューズ(拡散反射光)</param>
	/// <param name="specular">スペキュラー(鏡面反射光)</param>
	void CreatePlane(float width, float height, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

	/// <summary>
	/// テクスチャあり四角形板ポリゴンを作成
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="texName">テクスチャの名前</param>
	/// <param name="ambient">アンビエント(環境光)</param>
	/// <param name="diffuse">ディフューズ(拡散反射光)</param>
	/// <param name="specular">スペキュラー(鏡面反射光)</param>
	void CreatePlaneTex(float width, float height, const string& texturePath, const Vector3& ambient = { 0.5f,0.5f,0.5f }, const Vector3& diffuse = { 0.5f,0.5f,0.5f }, const Vector3& specular = { 0,0,0 });

#pragma region Getter
	/// <summary>
	/// 頂点配列を取得
	/// </summary>
	/// <returns>頂点配列</returns>
	inline const std::vector<Vertex>& GetVertices() { return vertices; }

	/// <summary>
	/// インデックス配列を取得
	/// </summary>
	/// <returns>インデックス配列</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices; }
#pragma endregion
};

