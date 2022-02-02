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
		bool isUse3D = true;
	};

private:
	static std::unordered_map<std::wstring, SoundData> sounds;
	static const float MASTER_DISTANCE;

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
	static void AddSound(const std::wstring& path, bool isUse3D = true, bool isLoop = false, float loopStartPos = 0, float loopEndPos = 1000);

	/// <summary>
	/// ���[�h�ω����f�[�^���Đ� (�G�~�b�^�w��Ȃ�)
	/// </summary>
	/// <param name="name">������(�g���q����)</param>
	static void Play(const std::wstring& name);
	/// <summary>
	/// ���[�h�ω����f�[�^���Đ�
	/// </summary>
	/// <param name="name">������(�g���q����)</param>
	/// <param name="emitterPos">���𔭂���3D��ԏ�ł̈ʒu</param>
	static void Play(const std::wstring& name, const Vector3& emitterPos);

	/// <summary>
	/// ���[�h�ω����f�[�^�̍Đ����~�߂�
	/// </summary>
	/// <param name="name">������(�g���q����)</param>
	static void Stop(const std::wstring& name);

	/// <summary>
	/// ���ݍĐ�����
	/// </summary>
	/// <param name="name">������(�g���q����)</param>
	/// <returns></returns>
	static bool IsPlaying(const std::wstring& name) { return sounds[name].sourceVoice.GetIsPlay(); }

	/// <summary>
	/// ���ʃZ�b�g
	/// </summary>
	/// <param name="name">������(�g���q����)</param>
	/// <param name="volume">���ʔ{��</param>
	static void SetVolume(const std::wstring& name, float volume);

	/// <summary>
	/// 3D�����Ɏg���G�~�b�^�[�̈ʒu�Z�b�g
	/// </summary>
	/// <param name="name">������(�g���q����)</param>
	/// <param name="pos">�ʒu</param>
	static void SetPosition(const std::wstring& name, const Vector3& pos);

	/// <summary>
	/// �ǂ��܂ŉ����������邩�̒l�Z�b�g �J�����ƒn�ʂƂ̋�����5�{���炢���悢
	/// </summary>
	/// <param name="distance"></param>
	static void SetDistance(float distance);

	/// <summary>
	/// ���[�h���Ă��鉹���t�@�C���擾
	/// </summary>
	/// <param name="name">������(�g���q����)</param>
	static SourceVoice& GetLoadedSound(const std::wstring& name);
};