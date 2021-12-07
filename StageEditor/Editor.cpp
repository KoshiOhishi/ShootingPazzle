#include <fstream>
#include "Editor.h"
#include "DX12Util.h"
#include "Input.h"
#include "Sound.h"
#include "DebugText.h"
#include "Collision.h"
#include "FPSManager.h"
#include "FbxLoader.h"
#include "ImguiHelper.h"
#include "GameUtility.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

void Editor::Initialize()
{
	//ライト初期化
	light.Initialize();
	light.SetLightDir({ 1,-1,0 });
	light.SetLightColor({ 0.8f, 0.8f, 0.8f });
	Object3D::SetLight(&light);

	//カメラ初期化
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,100.0f,0.0f }, 30.0f);
	camera.SetRotation(90, 0, 0);

	//カメラをセット
	Object3D::SetCamera(&camera);
	Mouse::SetCamera(&camera);

	//タイマーセット
	timer.SetTimer(0, 1000000);
	timer.Start();

	//ステージ取得
	stage.LoadStage("");
	sliderWidth = stage.stageSize.x;
	sliderDepth = stage.stageSize.y;

	//フェーズ初期化
	GameUtility::SetNowPhase(PHASE_SETPOS);

	//オブジェクト初期化
	squareBlock.Initialize({ 0,0 }, ONE_CELL_LENGTH / 2);
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].Initialize({ 0,0 }, ONE_CELL_LENGTH / 2);
		triangleBlock[i].SetShapeType(i);
	}
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].Initialize(&stage.stageSize);
	}
}

void Editor::Update()
{
	// DirectX 毎フレーム処理 ここから

	//入力処理ここから

#pragma region カメラ
#ifdef _DEBUG
	if (isEnteringIOName == false) {
		//カメラ移動
		if (Keyboard::IsKeyPush(DIK_W)) {
			camera.MoveCamera(0, 0, 1, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_S)) {
			camera.MoveCamera(0, 0, -1, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_A)) {
			camera.MoveCamera(-1, 0, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_D)) {
			camera.MoveCamera(1, 0, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_E)) {
			camera.MoveCamera(0, 1, 0, false, true, false);
		}
		if (Keyboard::IsKeyPush(DIK_X)) {
			camera.MoveCamera(0, -1, 0, false, true, false);
		}

		//カメラ回転
		if (Keyboard::IsKeyPush(DIK_UP)) {
			Vector3 f = { camera.GetRotation().x - 1.5f, camera.GetRotation().y, camera.GetRotation().z };
			camera.SetRotation(f);
		}
		if (Keyboard::IsKeyPush(DIK_DOWN)) {
			Vector3 f = { camera.GetRotation().x + 1.5f, camera.GetRotation().y, camera.GetRotation().z };
			camera.SetRotation(f);
		}
		if (Keyboard::IsKeyPush(DIK_LEFT)) {
			//Shiftが押されていたらZ軸回転
			if (Keyboard::IsKeyPush(DIK_LSHIFT) || Keyboard::IsKeyPush(DIK_RSHIFT)) {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z + 1.5f };
				camera.SetRotation(f);
			}
			else {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y - 1.5f, camera.GetRotation().z };
				camera.SetRotation(f);
			}
		}
		if (Keyboard::IsKeyPush(DIK_RIGHT)) {
			if (Keyboard::IsKeyPush(DIK_LSHIFT) || Keyboard::IsKeyPush(DIK_RSHIFT)) {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y, camera.GetRotation().z - 1.5f };
				camera.SetRotation(f);
			}
			else {
				Vector3 f = { camera.GetRotation().x, camera.GetRotation().y + 1.5f, camera.GetRotation().z };
				camera.SetRotation(f);
			}
		}
	}
#endif
#pragma endregion
#pragma region サウンド

#pragma endregion
#pragma region オブジェクト
	UpdateEdit();
#pragma endregion
#pragma region アップデート
	light.Update();
	camera.Update();
	timer.Update();
	//3Dサウンドで使用するリスナーの位置更新
	Sound::Set3DListenerPosAndVec(camera);
	//ステージ更新
	stage.Update();

#pragma endregion

#ifdef _DEBUG
	//リセット
	if (Keyboard::IsKeyTrigger(DIK_R) && isEnteringIOName == false) {
		Initialize();
	}

#endif


}

void Editor::Draw()
{
	//背景スプライト描画ここから
	Sprite::BeginDraw();
	//spr->Draw();

	//背景描画ここまで
	DX12Util::ClearDepthBuffer();

	//オブジェクト描画ここから

	DrawEdit();

	//オブジェクト描画ここまで

	//前景スプライト描画ここから
	DebugText::DrawAll();
	ImguiHelper::Draw();

	//前景スプライト描画ここまで

}

void Editor::UpdateEdit()
{
	UpdateImgui();
	CalcNowCursolPos();
	UpdateObject();

	if (mode == MODE_ADD) {
		UpdateAdd();
	}
	else if (mode == MODE_DELETE) {
		UpdateDelete();
	}

}

void Editor::UpdateAdd()
{
	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
		stage.AddBlock(nowCursolPos, blockType, shapeType);
	}
}

void Editor::UpdateDelete()
{
	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
		stage.DeleteBlock(nowCursolPos);
	}
}

void Editor::UpdateObject()
{
	//ブロック位置をカーソル位置に
	squareBlock.SetStagePos(nowCursolPos);
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].SetStagePos(nowCursolPos);
	}

	//更新
	UpdateStartLane();
	squareBlock.Update();
	for (int i = 0; i < _countof(triangleBlock); i++) {
		triangleBlock[i].Update();
	}
}

void Editor::UpdateStartLane()
{
	//0...設置されたスタートレーン
	startLane[0].SetPosition(stage.startLaneZ);
	startLane[0].Update();
	//1...カーソル位置に描画されるスタートレーン
	if (mode == MODE_OPTION && optionMode == OPTION_SET_STARTLANE) {
		//ステージ範囲外なら即リターン
		if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
			nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
			return;
		}
		startLane[1].SetPosition(nowCursolPos.y);

		if (Mouse::IsMouseButtonPush(MouseButton::LEFT)) {
			stage.startLaneZ = nowCursolPos.y;
		}
	}
	startLane[1].Update();
}

void Editor::DrawEdit()
{
	//ステージ描画
	stage.Draw();

	//スタートレーン描画
	DrawStartLane();

	if (mode == MODE_ADD) {
		//カーソル位置にブロック描画
		DrawBlock();
	}
	else if (mode == MODE_DELETE) {

	}
}

void Editor::DrawStartLane()
{
	//0...設置されたスタートレーン
	startLane[0].Draw();

	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	//1...カーソル位置に描画されるスタートレーン
	if (mode == MODE_OPTION && optionMode == OPTION_SET_STARTLANE) {
		startLane[1].Draw();
	}
}

void Editor::DrawBlock()
{
	//ステージ範囲外なら即リターン
	if (nowCursolPos.x < 0 || nowCursolPos.x >= stage.stageSize.x ||
		nowCursolPos.y < 0 || nowCursolPos.y >= stage.stageSize.y) {
		return;
	}

	if (blockType == BLOCKTYPE_SQUARE) {
		squareBlock.Draw();
	}
	else if (blockType == BLOCKTYPE_TRIANGLE) {
		triangleBlock[shapeType].Draw();
	}
}

void Editor::Save()
{
	std::ofstream file;
	//バイナリモードで開く
	file.open("./StageData/" + ioname + ".spb", std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//ファイルヘッダー
	StageHeader header = {};
	header.width = stage.stageSize.x;
	header.depth = stage.stageSize.y;
	header.startLineZ = stage.startLaneZ;
	header.objectCount = stage.blocks.size();

	file.write((char*)&header, sizeof(header));

	//データ部
	for (int i = 0; i < stage.blocks.size(); i++) {
		StageObject object = {};
		std::string blockType = stage.blocks[i]->GetObjectType();

		if (blockType == "SquareBlock") {
			object.type = 0;
		}
		else if (blockType == "TriangleBlock_0") {
			object.type = 1;
		}
		else if (blockType == "TriangleBlock_1") {
			object.type = 2;
		}
		else if (blockType == "TriangleBlock_2") {
			object.type = 3;
		}
		else if (blockType == "TriangleBlock_3") {
			object.type = 4;
		}

		StageVec2 pos = GameUtility::CalcWorldPos2StagePos(
			stage.blocks[i]->GetPosition().x, stage.blocks[i]->GetPosition().z);
		object.stagePosX = pos.x;
		object.stagePosY = pos.y;

		file.write((char*)&object, sizeof(object));
	}

	file.close();
}

void Editor::Load()
{
	stage.LoadStage("./StageData/" + ioname + ".spb");
	sliderWidth = stage.stageSize.x;
	sliderDepth = stage.stageSize.y;
}

void Editor::UpdateImgui()
{
	//ImGui
	ImguiHelper::BeginCommand("Settings");
	ImguiHelper::SetWindowSize(200, 720);
	ImguiHelper::SetWindowPos(1280 - 200, 0);
	ImGui::Text("Mode");
	ImGui::RadioButton("Add", &mode, MODE_ADD);
	ImGui::SameLine();
	ImGui::RadioButton("Delete", &mode, MODE_DELETE);
	ImGui::RadioButton("Option", &mode, MODE_OPTION);
	if (mode == MODE_ADD) {
		ImGui::Text("BlockType");
		ImGui::RadioButton("Square", &blockType, BLOCKTYPE_SQUARE);
		ImGui::SameLine();
		ImGui::RadioButton("Triangle", &blockType, BLOCKTYPE_TRIANGLE);

		if (blockType == BLOCKTYPE_TRIANGLE) {
			ImGui::Text("TriangleType");
			ImGui::RadioButton("No_LeftTop", &shapeType, SHAPETYPE_NO_LEFTTOP);
			ImGui::RadioButton("No_RightTop", &shapeType, SHAPETYPE_NO_RIGHTTOP);
			ImGui::RadioButton("No_LeftBottom", &shapeType, SHAPETYPE_NO_LEFTBOTTOM);
			ImGui::RadioButton("No_RightBottom", &shapeType, SHAPETYPE_NO_RIGHTBOTTOM);
		}
	}
	else if (mode == MODE_DELETE) {
		ImGui::NewLine();
		if (ImGui::Button("All Delete")) {
			//全て削除
			for (int i = 0; i < stage.blocks.size(); i++) {
					if (stage.blocks[i]) delete stage.blocks[i];
					stage.blocks.erase(stage.blocks.begin() + i);
					i--;
			}
		}
	}
	else if (mode == MODE_OPTION) {
		//ステージサイズ変更
		ImGui::SliderInt("StageWidth", &sliderWidth, 5, 50);
		ImGui::SliderInt("StageDepth", &sliderDepth, 5, 50);
		//大きさが変更されたら地面再生成
		if (sliderWidth != stage.stageSize.x || sliderDepth != stage.stageSize.y) {
			ReCreateStage(sliderWidth, sliderDepth);
		}
		//オプションモード選択
		ImGui::Text("OptionMode");
		ImGui::RadioButton("SetStartLane", &optionMode, OPTION_SET_STARTLANE);
	}

	ImGui::NewLine();

	static char c[128] = "test";
	ImGui::InputText("IO_Name", c, sizeof(c));
	ioname = c;
	isEnteringIOName = ImGui::IsItemActive();

	if (ImGui::Button("Save")) {
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		Load();
	}

	ImguiHelper::EndCommand();
}

void Editor::CalcNowCursolPos()
{
	Vector3 mouse;
	//マウスと地面との当たり判定
	Collision::CheckRay2Plane(Mouse::GetMouseRay(), stage.floor.GetPlane(), nullptr, &mouse);
	//DebugText::Print("WorldPos:(" + std::to_string(mouse.x) + ", " + std::to_string(mouse.z) + ")", 0, 0);
	StageVec2 newPos = GameUtility::CalcWorldPos2StagePos(mouse.x, mouse.z);
	nowCursolPos = newPos;

	//DebugText::Print("StagePos:(" + std::to_string(nowCursolPos.x) + ", " + std::to_string(nowCursolPos.y) + ")", 0, 20);
}

void Editor::ReCreateStage(unsigned short width, unsigned short depth)
{
	//まず変動したステージサイズに合わせてオブジェクト移動
	StageVec2 prev = stage.stageSize;
	for (int i = 0; i < stage.blocks.size(); i++) {
		//CalcWorldPos2StagePosでstage.stageSizeが参照されるので変更前の値を代入
		stage.stageSize = prev;
		Vector3 wpos = stage.blocks[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		//SetStagePosで変更後の座標にするために新ステージサイズを代入
		stage.stageSize.x = width;
		stage.stageSize.y = depth;
		stage.blocks[i]->SetStagePos(stagePos);
	}

	stage.stageSize.x = width;
	stage.stageSize.y = depth;

	//オブジェクトがステージ外に配置されていたら削除
	for (int i = 0; i < stage.blocks.size(); i++) {

		Vector3 wpos = stage.blocks[i]->GetPosition();
		StageVec2 stagePos = GameUtility::CalcWorldPos2StagePos(wpos.x, wpos.z);

		if (stagePos.x < 0 || stagePos.x >= stage.stageSize.x ||
			stagePos.y < 0 || stagePos.y >= stage.stageSize.y) {
			if (stage.blocks[i]) delete stage.blocks[i];
			stage.blocks.erase(stage.blocks.begin() + i);
			i--;
		}
	}

	//床モデル再生成
	stage.floor.CreateModel(stage.stageSize);
	stage.startLaneZ = stage.stageSize.y - 2;

	//スタートレーンモデル再生成
	for (int i = 0; i < _countof(startLane); i++) {
		startLane[i].CreateModel();
	}
}

//bool blnChk = false;
//ImGui::Checkbox("CheckboxTest", &blnChk);
//int radio = 0;
//ImGui::RadioButton("Radio 1", &radio, 0);
//ImGui::SameLine();
//ImGui::RadioButton("Radio 2", &radio, 1);
//ImGui::SameLine();
//ImGui::RadioButton("Radio 3", &radio, 2);
//int nSlider = 0;
//ImGui::SliderInt("Int Slider", &nSlider, 0, 100);
//float fSlider = 0.0f;
//ImGui::SliderFloat("Float Slider", &fSlider, 0.0f, 100.0f);
//float col3[3] = {};
//ImGui::ColorPicker3("ColorPicker3", col3, ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
//float col4[4] = {};
//ImGui::ColorPicker4(" ColorPicker4", col4, ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | 
//ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

