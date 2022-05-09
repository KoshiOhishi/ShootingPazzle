#pragma once
#include "GameUtility.h"
#include "EditorStage.h"
#include "Timer.h"
#include <vector>

//コマンドの種類
enum CommandType {
	COMMAND_TYPE_ADD,
	COMMAND_TYPE_DELETE,
	COMMAND_TYPE_STAGE_WIDTH,
	COMMAND_TYPE_STAGE_HEIGHT,
	COMMAND_TYPE_SET_STARTLANE
};

//コマンド内容
struct CommandDetail {
	int commandType = -1;		//種類
	int args1 = -1;				//Add,Delete...x座標 そのほかは変更前の位置や大きさ
	int args2 = -1;				//Add,Delete...y座標 そのほかは変更後の位置や大きさ
	int objectType = -1;		//ブロックか床か
	int blockFloorType = -1;	//ブロックor床の種類
	int breakupCount = -1;		//破壊可能までの回数
	int colorType = -1;			//色の種類

	//一部比較演算のオペレータ
	bool operator == (const CommandDetail& d)const {
		bool b1 = commandType == d.commandType;
		bool b2 = args1 == d.args1;
		bool b3 = args2 == d.args2;
		bool b4 = objectType == d.objectType;
		bool b5 = blockFloorType == d.blockFloorType;
		bool b6 = breakupCount == d.breakupCount;
		bool b7 = colorType == d.colorType;

		return b1 && b2 && b3 && b4 && b5 && b6 && b7;
	}

	//一部比較演算のオペレータ
	bool operator != (const CommandDetail& d)const {
		bool b1 = commandType != d.commandType;
		bool b2 = args1 != d.args1;
		bool b3 = args2 != d.args2;
		bool b4 = objectType != d.objectType;
		bool b5 = blockFloorType != d.blockFloorType;
		bool b6 = breakupCount != d.breakupCount;
		bool b7 = colorType != d.colorType;

		return b1 || b2 || b3 || b4 || b5 || b6 || b7;
	}
};

//コマンド管理、適用クラス
class Command
{
private:
	//コマンド格納コンテナ
	static std::vector<CommandDetail> commands;
	//編集するステージのポインタ
	static EditorStage* pStage;
	//現在のコマンド位置
	static int commandPos;
	//長押しタイマー
	static DX12Library::Timer longPushTimer;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// 更新　Ctrl+Zの適用などはここ
	/// </summary>
	static void Update();

	/// <summary>
	/// コマンド追加
	/// </summary>
	/// <param name="commandDetail"></param>
	static void AddCommand(const CommandDetail& commandDetail);

	/// <summary>
	/// 編集するステージのポインタセット
	/// </summary>
	/// <param name="pStage">ステージのポインタ</param>
	static void SetPStage(EditorStage* pStage) { Command::pStage = pStage; }

private:
	/// <summary>
	/// タイマー更新
	/// </summary>
	static void UpdateTimer();

	/// <summary>
	/// Ctrl + Zが押されたかを返す
	/// </summary>
	/// <returns>Ctrl + Zが押されたか</returns>
	static bool IsUndo();

	/// <summary>
	/// Ctrl + Yが押されたかを返す
	/// </summary>
	/// <returns>Ctrl + Yが押されたか</returns>
	static bool IsRedo();

	/// <summary>
	/// アンドゥを適用
	/// </summary>
	static void ApplyUndo();

	/// <summary>
	/// リドゥを適用
	/// </summary>
	static void ApplyRedo();

	/// <summary>
	/// コマンド実行
	/// </summary>
	/// <param name="commandDetail"></param>
	static void ApplyCommand(const CommandDetail& commandDetail);

	/// <summary>
	/// 入力されたコマンドの逆操作のコマンドを返す アンドゥ用
	/// </summary>
	/// <param name="src">元コマンド</param>
	/// <returns>元コマンドの逆操作</returns>
	static CommandDetail GetReverseCommand(const CommandDetail& src);
};

