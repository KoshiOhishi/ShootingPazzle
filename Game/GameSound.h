#pragma once
#include "Sound.h"
#include "Timer.h"
#include <unordered_map>
class GameSound
{
	struct SoundData
	{
		WaveData waveData;
		SourceVoice sourceVoice;
		Timer stopTimer;
	};

private:
	static std::unordered_map<std::wstring, SoundData> sounds;

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �X�V
	/// </summary>
	static void Update();

	/// <summary>
	/// �����f�[�^�ǉ�
	/// </summary>
	/// <param name="path"></param>
	static void AddSound(const std::wstring& path, bool isLoop = false, float loopStartPos = 0, float loopEndPos = 1000);

	/// <summary>
	/// ���[�h�ω����f�[�^���Đ�
	/// </summary>
	/// <param name="name"></param>
	static void Play(const std::wstring& name, const Vector3& emitterPos);

	/// <summary>
	/// ���[�h�ω����f�[�^�̍Đ����~�߂�
	/// </summary>
	/// <param name="name"></param>
	static void Stop(const std::wstring& name);

	/// <summary>
	/// ���ʃZ�b�g
	/// </summary>
	/// <param name="name"></param>
	/// <param name="volume">���ʔ{��</param>
	static void SetVolume(const std::wstring& name, float volume);

	/// <summary>
	/// 3D�����Ɏg���G�~�b�^�[�̈ʒu�Z�b�g
	/// </summary>
	/// <param name="pos"></param>
	static void SetPosition(const std::wstring& name, const Vector3& pos);

	/// <summary>
	/// ���[�h���Ă��鉹���t�@�C���擾
	/// </summary>
	/// <param name="name"></param>
	static SourceVoice& GetLoadedSound(const std::wstring& name);

};