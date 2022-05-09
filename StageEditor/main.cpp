#include <Windows.h>
#include <wrl.h>
#include "DX12Util.h"
#include "SceneManager.h"
#include "Editor.h"
#include "GameSound.h"

using namespace DX12Library;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//初期化
	DX12Util::Initialize(L"Stage Editor", 1280, 720);

	GameSound::StaticInitialize();

	//シーンセット
	SceneManager::AddScene(new Editor(), "Editor");
	SceneManager::ChangeScene("Editor");

	GameUtility::StaticInitialize();

	MSG msg{}; // メッセージ
	while (true)
	{
		// メッセージがある?
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg); // キー入力メッセージの処理
			DispatchMessage(&msg); // プロシージャにメッセージを送る
		}
		// 終了メッセージが来たらループを抜ける
		if (msg.message == WM_QUIT) {
			break;
		}

		DX12Util::Update();
	}

	GameSound::StaticFinalize();
	DX12Util::End();

	return 0;
}