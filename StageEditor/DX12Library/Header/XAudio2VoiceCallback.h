#pragma once
#include <xaudio2.h>

class XAudio2VoiceCallback :
	public IXAudio2VoiceCallback
{

private:
	HANDLE hbufferEndEvent;

public:
	//�{�C�X�����p�X�̊J�n��
	void OnVoiceProcessingPassStart(UINT32 BytesRequired) {}
	//�{�C�X�����p�X�̏I����
	void OnVoiceProcessingPassEnd() {}
	//�o�b�t�@�X�g���[���̍Đ����I��������
	void OnStreamEnd() {}
	//�o�b�t�@�̎g�p�J�n��
	void OnBufferStart(void* pBufferContext) {}
	//�o�b�t�@�̖����ɒB�����Ƃ�
	void OnBufferEnd(void* pBufferContext) {
		CreateEvent(NULL, FALSE, FALSE, NULL);
		//delete[] pBufferContext;
	}
	//�Đ������[�v�ʒu�ɒB������
	void OnLoopEnd(void* pBufferContext) {}
	//�{�C�X�̎��s�G���[��
	void OnVoiceError(void* pBufferContext, HRESULT Error) {}
};

