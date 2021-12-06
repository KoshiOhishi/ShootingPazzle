#pragma once
#include <xaudio2.h>

class XAudio2VoiceCallback :
	public IXAudio2VoiceCallback
{

private:
	HANDLE hbufferEndEvent;

public:
	//ボイス処理パスの開始時
	void OnVoiceProcessingPassStart(UINT32 BytesRequired) {}
	//ボイス処理パスの終了時
	void OnVoiceProcessingPassEnd() {}
	//バッファストリームの再生が終了した時
	void OnStreamEnd() {}
	//バッファの使用開始時
	void OnBufferStart(void* pBufferContext) {}
	//バッファの末尾に達したとき
	void OnBufferEnd(void* pBufferContext) {
		CreateEvent(NULL, FALSE, FALSE, NULL);
		//delete[] pBufferContext;
	}
	//再生がループ位置に達した時
	void OnLoopEnd(void* pBufferContext) {}
	//ボイスの実行エラー時
	void OnVoiceError(void* pBufferContext, HRESULT Error) {}
};

