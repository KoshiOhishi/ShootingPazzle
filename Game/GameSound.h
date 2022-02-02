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
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	/// <summary>
	/// 音声データ追加
	/// </summary>
	/// <param name="path"></param>
	static void AddSound(const std::wstring& path, bool isUse3D = true, bool isLoop = false, float loopStartPos = 0, float loopEndPos = 1000);

	/// <summary>
	/// ロード済音声データを再生 (エミッタ指定なし)
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	static void Play(const std::wstring& name);
	/// <summary>
	/// ロード済音声データを再生
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <param name="emitterPos">音を発する3D空間上での位置</param>
	static void Play(const std::wstring& name, const Vector3& emitterPos);

	/// <summary>
	/// ロード済音声データの再生を止める
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	static void Stop(const std::wstring& name);

	/// <summary>
	/// 現在再生中か
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <returns></returns>
	static bool IsPlaying(const std::wstring& name) { return sounds[name].sourceVoice.GetIsPlay(); }

	/// <summary>
	/// 音量セット
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <param name="volume">音量倍率</param>
	static void SetVolume(const std::wstring& name, float volume);

	/// <summary>
	/// 3D音響に使うエミッターの位置セット
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	/// <param name="pos">位置</param>
	static void SetPosition(const std::wstring& name, const Vector3& pos);

	/// <summary>
	/// どこまで音を響かせるかの値セット カメラと地面との距離の5倍くらいがよい
	/// </summary>
	/// <param name="distance"></param>
	static void SetDistance(float distance);

	/// <summary>
	/// ロードしてある音声ファイル取得
	/// </summary>
	/// <param name="name">音源名(拡張子無し)</param>
	static SourceVoice& GetLoadedSound(const std::wstring& name);
};