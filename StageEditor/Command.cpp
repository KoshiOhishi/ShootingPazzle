#include "Command.h"
#include "Input.h"
#include "DebugText.h"

//コマンド格納コンテナ
std::vector<CommandDetail> Command::commands;
//編集するステージのポインタ
EditorStage* Command::pStage;
//現在のコマンド位置
int Command::commandPos = -1;
//長押しタイマー
Timer Command::longPushTimer;

void Command::Initialize()
{
	//コマンド全クリア
	commands.clear();
	commandPos = -1;

	//タイマー初期化
	longPushTimer.SetTimer(0, 300);
}

void Command::Update()
{
	//タイマー更新
	UpdateTimer();

	//アンドゥ入力
	if (IsUndo()) {
		//アンドゥ適用
		ApplyUndo();
	}

	//リドゥ入力
	if (IsRedo()) {
		//リドゥ適用
		ApplyRedo();
	}
}

void Command::AddCommand(const CommandDetail& commandDetail)
{

	//コマンドの位置が末尾なら
	if (commandPos == commands.size() - 1) {
		//同じ内容のコマンドが既に末尾に存在していたら追加しない
		if (commands.size() > 0 && commandDetail == commands[commands.size() - 1]) {
			return;
		}
	}
	//コマンドの位置が末尾でないなら
	else {
		//末尾からコマンドの位置までの数削除
		int count = commands.size() - 1 - commandPos;
		for (int i = 0; i < count; i++) {
			commands.pop_back();
		}
	}

	//追加
	commands.emplace_back(commandDetail);

	//コマンド位置を末尾に
	commandPos = commands.size() - 1;
}

void Command::UpdateTimer()
{
	longPushTimer.Update();

	//アンドゥ/リドゥキーが押されていたらタイマースタート
	if ((Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && 
		(Keyboard::IsKeyPush(DIK_Z) || Keyboard::IsKeyPush(DIK_Y))) {
		if (longPushTimer.GetNowTime() <= longPushTimer.GetStartTime()) {
			longPushTimer.Start();
		}
	}
	else {
		longPushTimer.Reset();
	}
}

bool Command::IsUndo()
{
	//長押しされているかで処理を分ける
	if (longPushTimer.GetIsEnd()) {
		//左右どちらかのCtrlが押されていて、Zも押されているか
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyPush(DIK_Z);
	}
	else {
		//左右どちらかのCtrlが押されていて、Zが押された瞬間か
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyTrigger(DIK_Z);
	}
}

bool Command::IsRedo()
{
	//長押しされているかで処理を分ける
	if (longPushTimer.GetIsEnd()) {
		//左右どちらかのCtrlが押されていて、Yも押されているか
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyPush(DIK_Y);
	}
	else {
		//左右どちらかのCtrlが押されていて、Yが押された瞬間か
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyTrigger(DIK_Y);
	}
}

void Command::ApplyUndo()
{
	//コマンド位置が-1だったら実行無し
	if (commandPos == -1) {
		return;
	}

	//コマンド位置の逆コマンドを実行
	ApplyCommand(GetReverseCommand(commands[commandPos]));
	//コマンド位置を1つ前に
	commandPos--;
}

void Command::ApplyRedo()
{
	//コマンド位置が末尾だったら実行なし
	if (commandPos == commands.size() - 1) {
		return;
	}

	//コマンド位置を1つ後ろに
	commandPos++;

	//コマンド位置のコマンドを実行
	ApplyCommand(commands[commandPos]);
}

void Command::ApplyCommand(const CommandDetail& commandDetail)
{
	//位置
	StageVec2 pos = { (unsigned short)commandDetail.args1, (unsigned short)commandDetail.args2 };

	//種類別にコマンド実行
	switch (commandDetail.commandType) {

	case COMMAND_TYPE_ADD:
		//種類別に追加
		if (commandDetail.objectType == OBJECTTYPE_BLOCK) {
			pStage->AddBlock(pos, commandDetail.blockFloorType, commandDetail.breakupCount, commandDetail.colorType, false);//コマンド追加無し
		}
		else if (commandDetail.objectType == OBJECTTYPE_FLOOR) {
			pStage->AddFloor(pos, commandDetail.blockFloorType, false);//コマンド追加無し
		}
		break;

	case COMMAND_TYPE_DELETE:
		//種類別に削除
		if (commandDetail.objectType == OBJECTTYPE_BLOCK) {
			pStage->DeleteBlock(pos, false);//コマンド追加無し
		}
		else if (commandDetail.objectType == OBJECTTYPE_FLOOR) {
			pStage->DeleteFloor(pos, false);//コマンド追加無し
		}
		break;

	case COMMAND_TYPE_STAGE_WIDTH:
		//変更後の値を適用
		pStage->stageSize.x = commandDetail.args2;
		break;
	case COMMAND_TYPE_STAGE_HEIGHT:
		//変更後の値を適用
		pStage->stageSize.y = commandDetail.args2;
		break;
	case COMMAND_TYPE_SET_STARTLANE:
		pStage->startLaneZ = commandDetail.args2;
		break;
	}
}

CommandDetail Command::GetReverseCommand(const CommandDetail& src)
{
	CommandDetail result = {};

	//コマンドの種類の逆を設定
	switch (src.commandType) {

	//追加→削除
	case COMMAND_TYPE_ADD:
		result.commandType = COMMAND_TYPE_DELETE;
		break;

	//削除→追加
	case COMMAND_TYPE_DELETE:
		result.commandType = COMMAND_TYPE_ADD;
		break;

	//そのほかは元と同じ種類
	case COMMAND_TYPE_STAGE_WIDTH:
	case COMMAND_TYPE_STAGE_HEIGHT:
	case COMMAND_TYPE_SET_STARTLANE:
		result.commandType = src.commandType;
		break;
	}

	//コマンドの内容の逆設定
	switch (result.commandType) {

	//追加、削除は位置なのでそのままコピー
	case COMMAND_TYPE_ADD:
	case COMMAND_TYPE_DELETE:
		result.args1 = src.args1;
		result.args2 = src.args2;
		break;

	//そのほかは変更前と変更後の値なので入れ替える
	case COMMAND_TYPE_STAGE_WIDTH:
	case COMMAND_TYPE_STAGE_HEIGHT:
	case COMMAND_TYPE_SET_STARTLANE:
		result.args1 = src.args2;
		result.args2 = src.args1;
		break;
	}

	//(追加と削除のみ)オブジェクトの種類コピー
	if (result.commandType == COMMAND_TYPE_ADD ||
		result.commandType == COMMAND_TYPE_DELETE) {
		result.objectType = src.objectType;
		result.blockFloorType = src.blockFloorType;
		result.breakupCount = src.breakupCount;
		result.colorType = src.colorType;
	}

	return result;
}
