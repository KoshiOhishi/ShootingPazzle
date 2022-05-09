#pragma once
#include <vector>
#include "BaseBlock.h"
#include "BaseFloor.h"
#include "GameUtility.h"

class EditorStage
{
//エディット用なので公開でOK
public:
	//球の大きさ
	const float sphereRadius = ONE_CELL_LENGTH / 2;
	//ステージサイズ
	StageVec2 stageSize = { 20,20 };
	//ブロックコンテナ
	std::vector<std::unique_ptr<BaseBlock>> blocks;
	//床ブロックコンテナ
	std::vector< std::unique_ptr<BaseFloor>> floors;
	//床当たり判定(マウス用)
	DX12Library::Plane floorCollision;
	//スタート縦位置
	unsigned short startLaneZ = stageSize.y - 2;

	//以下表示用オブジェクト
	DX12Library::InstancingObjectDraw iodSquareBlock[4];
	DX12Library::InstancingObjectDraw iodTriangleBlock[4];
	DX12Library::InstancingObjectDraw iodNormalFloor;
	DX12Library::InstancingObjectDraw iodSwitchFloor[2];
	DX12Library::InstancingObjectDraw iodTurnFloor[8];
	DX12Library::InstancingObjectDraw iodBreakFloor;

public:
	EditorStage(){}
	~EditorStage();

	void Initialize();

	/// <summary>
	/// ステージを読み込んで初期化 ファイルが見つからない場合は20x20のブロックなしを生成
	/// </smmary>
	/// <param name="filename">ファイルパス</param>
	void LoadStage(std::string filename);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ブロックを追加 (引数の位置に既にブロックが配置されていたら追加しない)
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <param name="blockType">ブロックの種類</param>
	/// <param name="breakupCount">壊れるまでの衝突回数　0で壊れないブロック</param>
	/// <param name="blockColor">ブロックの色指定</param>
	void AddBlock(const StageVec2& stagePos, int blockType, unsigned short breakupCount, int blockColor, bool addCommand = true);

	/// <summary>
	/// ブロックを削除 (引数の位置になにもない場合は削除しない)
	/// </summary>
	/// <param name="stagePos"></param>
	void DeleteBlock(const StageVec2& stagePos, bool addCommand = true);

	/// <summary>
	/// 既にブロックが配置されているかチェック
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <returns>存在すればブロック配列のインデックス、なければ-1を返す</returns>
	int CheckExistBlock(const StageVec2& stagePos);

	/// <summary>
	/// 床ブロックを追加 (引数の位置に既にブロックが配置されていたら追加しない)
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <param name="floorType">床ブロックの種類</param>
	void AddFloor(const StageVec2& stagePos, int floorType, bool addCommand = true);

	/// <summary>
	/// 床ブロックを削除 (引数の位置になにもない場合は削除しない)
	/// </summary>
	/// <param name="stagePos"></param>
	void DeleteFloor(const StageVec2& stagePos, bool addCommand = true);

	/// <summary>
	/// 既に床ブロックが配置されているかチェック
	/// </summary>
	/// <param name="stagePos">ステージ上の座標</param>
	/// <returns>存在すればブロック配列のインデックス、なければ-1を返す</returns>
	int CheckExistFloor(const StageVec2& stagePos);

	const DX12Library::Plane& GetFloorCollision()const { return floorCollision; }

};

