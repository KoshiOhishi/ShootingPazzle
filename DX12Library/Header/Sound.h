#pragma once
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <wrl.h>
#include <cstdint>
#include "XAudio2VoiceCallback.h"
#include <string>
#include <unordered_map>
#include <x3daudio.h>
#include <DirectXMath.h>
#include "Camera.h"
#include <memory>

#pragma comment(lib, "xaudio2.lib")

#pragma region Define
#pragma region from "ksmedia.h"
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000
#define SPEAKER_RESERVED                0x7FFC0000
#define SPEAKER_ALL                     0x80000000

#define KSAUDIO_SPEAKER_MONO            (SPEAKER_FRONT_CENTER)
#define KSAUDIO_SPEAKER_1POINT1         (SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_STEREO          (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)
#define KSAUDIO_SPEAKER_2POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_3POINT0         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER)
#define KSAUDIO_SPEAKER_3POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY)
#define KSAUDIO_SPEAKER_QUAD            (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT)
#define KSAUDIO_SPEAKER_SURROUND        (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER)
#define KSAUDIO_SPEAKER_5POINT0         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | \
                                         SPEAKER_SIDE_LEFT  | SPEAKER_SIDE_RIGHT)
#define KSAUDIO_SPEAKER_5POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT)
#define KSAUDIO_SPEAKER_7POINT0         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | \
                                         SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT)
#define KSAUDIO_SPEAKER_7POINT1         (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | \
                                         SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | \
                                         SPEAKER_FRONT_LEFT_OF_CENTER | SPEAKER_FRONT_RIGHT_OF_CENTER)
#pragma endregion

#pragma region reverb_preset
// デフォルト(意味なし？)
#define DX12LIBSOUND_REBERB_PRESET_DEFAULT XAUDIO2FX_I3DL2_PRESET_DEFAULT
// 一般的な空間
#define DX12LIBSOUND_REBERB_PRESET_GENERIC XAUDIO2FX_I3DL2_PRESET_GENERIC
// 精神病患者室
#define DX12LIBSOUND_REBERB_PRESET_PADDEDCELL XAUDIO2FX_I3DL2_PRESET_PADDEDCELL
// 部屋
#define DX12LIBSOUND_REBERB_PRESET_ROOM XAUDIO2FX_I3DL2_PRESET_ROOM
// バスルーム
#define DX12LIBSOUND_REBERB_PRESET_BATHROOM XAUDIO2FX_I3DL2_PRESET_BATHROOM
// リビングルーム
#define DX12LIBSOUND_REBERB_PRESET_LIVINGROOM XAUDIO2FX_I3DL2_PRESET_LIVINGROOM
// 石の部屋
#define DX12LIBSOUND_REBERB_PRESET_STONEROOM XAUDIO2FX_I3DL2_PRESET_STONEROOM
// 講堂
#define DX12LIBSOUND_REBERB_PRESET_AUDITORIUM XAUDIO2FX_I3DL2_PRESET_AUDITORIUM
// コンサートホール
#define DX12LIBSOUND_REBERB_PRESET_CONCERTHALL XAUDIO2FX_I3DL2_PRESET_CONCERTHALL
// 洞穴
#define DX12LIBSOUND_REBERB_PRESET_CAVE XAUDIO2FX_I3DL2_PRESET_CAVE
// 舞台
#define DX12LIBSOUND_REBERB_PRESET_ARENA XAUDIO2FX_I3DL2_PRESET_ARENA
// 格納庫
#define DX12LIBSOUND_REBERB_PRESET_HANGAR XAUDIO2FX_I3DL2_PRESET_HANGAR
// カーペットが敷かれた玄関
#define DX12LIBSOUND_REBERB_PRESET_CARPETEDHALLWAY XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY
// 玄関
#define DX12LIBSOUND_REBERB_PRESET_HALLWAY XAUDIO2FX_I3DL2_PRESET_HALLWAY
// 石の廊下
#define DX12LIBSOUND_REBERB_PRESET_STONECORRIDOR XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR
// 裏通り
#define DX12LIBSOUND_REBERB_PRESET_ALLEY XAUDIO2FX_I3DL2_PRESET_ALLEY
// 森
#define DX12LIBSOUND_REBERB_PRESET_FOREST XAUDIO2FX_I3DL2_PRESET_FOREST
// 都市
#define DX12LIBSOUND_REBERB_PRESET_CITY XAUDIO2FX_I3DL2_PRESET_CITY
// 山
#define DX12LIBSOUND_REBERB_PRESET_MOUNTAINS XAUDIO2FX_I3DL2_PRESET_MOUNTAINS
// 採石場
#define DX12LIBSOUND_REBERB_PRESET_QUARRY XAUDIO2FX_I3DL2_PRESET_QUARRY
// 平原
#define DX12LIBSOUND_REBERB_PRESET_PLAIN XAUDIO2FX_I3DL2_PRESET_PLAIN
// 駐車場
#define DX12LIBSOUND_REBERB_PRESET_PARKINGLOT XAUDIO2FX_I3DL2_PRESET_PARKINGLOT
// 下水管
#define DX12LIBSOUND_REBERB_PRESET_SEWERPIPE XAUDIO2FX_I3DL2_PRESET_SEWERPIPE
// 水面下
#define DX12LIBSOUND_REBERB_PRESET_UNDERWATER XAUDIO2FX_I3DL2_PRESET_UNDERWATER
// 小部屋
#define DX12LIBSOUND_REBERB_PRESET_SMALLROOM XAUDIO2FX_I3DL2_PRESET_SMALLROOM
// 中部屋
#define DX12LIBSOUND_REBERB_PRESET_MEDIUMROOM XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM
// 大部屋
#define DX12LIBSOUND_REBERB_PRESET_LARGEROOM XAUDIO2FX_I3DL2_PRESET_LARGEROOM
// 中ホール
#define DX12LIBSOUND_REBERB_PRESET_MEDIUMHALL XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL
// 大ホール
#define DX12LIBSOUND_REBERB_PRESET_LARGEHALL XAUDIO2FX_I3DL2_PRESET_LARGEHALL
// 板
#define DX12LIBSOUND_REBERB_PRESET_PLATE XAUDIO2FX_I3DL2_PRESET_PLATE
#pragma endregion

#pragma region Effect_Filter_Type

//ローパスフィルタ
#define DX12LIBSOUND_EFFECT_FILTER_LOWPASS XAUDIO2_FILTER_TYPE::LowPassFilter
//バンドパスフィルタ
#define DX12LIBSOUND_EFFECT_FILTER_BANDPASS XAUDIO2_FILTER_TYPE::BandPassFilter
//ハイパスフィルタ
#define DX12LIBSOUND_EFFECT_FILTER_HIGHPASS XAUDIO2_FILTER_TYPE::HighPassFilter
//ノッチフィルタ
#define DX12LIBSOUND_EFFECT_FILTER_NOTCHPASS XAUDIO2_FILTER_TYPE::NotchFilter
//単極ローパスフィルタ
#define DX12LIBSOUND_EFFECT_FILTER_LOWPASS_ONEPOLE XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter
//単極ハイパスフィルタ
#define DX12LIBSOUND_EFFECT_FILTER_HIGHPASS_ONEPOLE XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter

#pragma endregion
#pragma endregion

namespace DX12Library {

	class XAudio2VoiceCallback;

	//エフェクトクラス
	class XAPOEffect
	{
		//エイリアス
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

	private:
		//デスクリプタ
		XAUDIO2_EFFECT_DESCRIPTOR effectDesc;
		//リバーブエフェクトか否か
		bool isReverb = false;

	public:
		~XAPOEffect();

		/// <summary>
		/// リバーブエフェクト生成
		/// </summary>
		/// <param name="initialState">初期適用状態</param>
		void CreateReverb(bool initialState = true);

		/// <summary>
		/// XAPOEffectを破棄する
		/// </summary>
		void DestroyXAPOEffect();

		/// <summary>
		/// デスクリプタ取得
		/// </summary>
		/// <returns></returns>
		XAUDIO2_EFFECT_DESCRIPTOR GetEffectDescriptor() { return effectDesc; }

		/// <summary>
		/// リバーブエフェクトかどうかを返す
		/// </summary>
		/// <returns></returns>
		bool IsReverb() { return isReverb; }
	};

	//サブミックスボイスクラス
	class SubmixVoice
	{
		//エイリアス
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

	private:
		//サブミックスボイス
		IXAudio2SubmixVoice* pSubmixVoice;
		//サブミックスボイスに適用中のエフェクトのキーを格納
		unordered_map<XAPOEffect*, int> effectIndexSubmix;

	public:
		~SubmixVoice();

		/// <summary>
		/// サブミックスボイスを生成する
		/// </summary>
		/// <param name="channelCount">チャンネル数</param>
		/// <param name="samplingRate">サンプリングレート</param>
		void CreateSubmixVoice(const int channelCount, const float samplingRate);

		/// <summary>
		/// サブミックスボイスを破棄する
		/// </summary>
		void DestroySubmixVoice();

		/// <summary>
		/// サブミックスボイスにエフェクト登録
		/// </summary>
		/// <param name="pEffect">適用するエフェクトのポインタ</param>
		void AddXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// エフェクトリストを空にする
		/// </summary>
		void ClearXAPOEffect();

		/// <summary>
		/// AddXAPOEffectで登録したXAPOEffectを有効にする
		/// </summary>
		/// <param name="pEffect">AddXAPOEffectで登録したXAPOEffectのポインタ</param>
		void EnableXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// AddXAPOEffectで登録したXAPOEffectを無効にする
		/// </summary>
		/// <param name="pEffect">AddXAPOEffectで登録したXAPOEffectのポインタ</param>
		void DisableXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// サブミックスボイスに適用中のリバーブエフェクトにプリセットからパラメータをセット
		/// </summary>
		/// <param name="pEffect">適用するエフェクトのポインタ</param>
		/// <param name="preset">REVERB_PRESETから始まるプリセット定義 (DX12LIBSOUND_REBERB_PRESET_...)</param>
		void SetReverbParamsFromPreset(XAPOEffect* pEffect, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& preset);

		/// <summary>
		/// サブミックスボイスに適用中のリバーブエフェクトのパラメータをセット
		/// </summary>
		/// <param name="pEffect">適用するエフェクトのポインタ</param>
		/// <param name="p">パラメータ配列のポインタ nullptrを渡すと初期設定になる</param>
		void SetReverbParams(XAPOEffect* pEffect, XAUDIO2FX_REVERB_PARAMETERS* p);

		/// <summary>
		/// 音量をセットする
		/// </summary>
		/// <param name="volume">音量値(倍率指定)</param>
		void SetVolume(const float volume);

		/// <summary>
		/// サブミックスボイスポインタ取得
		/// </summary>
		/// <returns></returns>
		IXAudio2SubmixVoice* GetPXAudio2SubmixVoice() { return pSubmixVoice; }

	};

	class WaveData
	{
		//エイリアス
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

		//メンバ変数
	private:
		//フォーマット
		std::unique_ptr<WAVEFORMATEX> wfex;
		std::unique_ptr<char> soundBuffer;
		UINT32 audioBytes;

		//メンバ関数
	public:
		~WaveData();

		/// <summary>
		/// wavファイルをロードする
		/// </summary>
		/// <param name="filename">ファイルパス</param>
		void LoadWave(const std::wstring& filename);

		/// <summary>
		/// 音声データを削除する ※音源破棄後に呼び出す！！
		/// </summary>
		void DeleteWave();

		/// <summary>
		/// 音声の長さを取得
		/// </summary>
		/// <returns>音声の長さ(ms)</returns>
		float GetSoundLength();

		/// <summary>
		/// 音源のサンプリングレートを取得
		/// </summary>
		/// <returns>サンプリングレートを</returns>
		float GetSamplingRate();

		/// <summary>
		/// 音源のチャンネル数を取得
		/// </summary>
		/// <returns>チャンネル数</returns>
		int GetChannelCount();

		/// <summary>
		/// WAVEFORMATEX取得
		/// </summary>
		/// <returns>WAVEFORMATEX</returns>
		WAVEFORMATEX* GetPWaveFormatEx() { return wfex.get(); }

		char* GetSoundBuffer() { return soundBuffer.get(); }

		UINT32 GetAudioBytes() { return audioBytes; }


	};

	//ソースボイスクラス
	class SourceVoice
	{
		//エイリアス
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

		//メンバ変数
	private:

		//ソースボイスのポインタ
		IXAudio2SourceVoice* pSourceVoice;
		//コールバックオブジェクト
		XAudio2VoiceCallback callback;
		//音源データポインタ
		WaveData* pWaveData;
		//音源ポインタ
		XAUDIO2_BUFFER buf = {};

		XAUDIO2_VOICE_DETAILS voiceDetails = {};

		//エフェクトを使用するか
		bool isUseEffect = false;
		//最大ピッチレート
		float maxPitchRate = 2.0f;
		//一時停止中か
		bool isPause = false;

		//ピッチ
		float pitch = 1.0f;

		//登録中のエフェクトのキーを格納
		unordered_map<XAPOEffect*, int> effectIndex;

		//出力先ボイスリスト
		std::vector<SubmixVoice*> submixVoiceList;


		//X3DAudio
		//Emitter
		X3DAUDIO_EMITTER emitter;
		X3DAUDIO_CONE emitterCone;
		X3DAUDIO_DSP_SETTINGS dspSettings;
		XMFLOAT3 emitterPrevPos;

		FLOAT32 azimuths[2];
		FLOAT32 matrix[2];

		//メンバ関数
	public:
		~SourceVoice();

		/// <summary>
		/// ソースボイスを生成する
		/// </summary>
		/// <param name="wavData">音源データ</param>
		void CreateSourceVoice(WaveData* pWaveData, bool isUseEffect = false, const float maxPitchRate = 2.0f);

		/// <summary>
		/// ソースボイスを破棄する
		/// </summary>
		void DestroySourceVoice();

		/// <summary>
		/// 再生する
		/// </summary>
		void PlayWave();

		/// <summary>
		/// 停止する
		/// </summary>
		void StopWave();

		/// <summary>
		/// 一時停止する
		/// </summary>
		void PauseWave();

		/// <summary>
		/// X3DAudioのCalculateと計算結果適用
		/// </summary>
		void Update3DAudio();

		/// <summary>
		/// フィルターエフェクトを無効化する
		/// </summary>
		void DisableFilterEffect();

		/// <summary>
		/// 出力先サブミックスボイスを追加
		/// </summary>
		/// <param name="pSubmixVoice">生成したサブミックスボイスのポインタ</param>
		void AddOutputSubmixVoice(SubmixVoice* pSubmixVoice);

		/// <summary>
		/// 出力先サブミックスボイスをクリア
		/// </summary>
		void ClearOutputSubmixVoice();

		/// <summary>
		/// ソースボイスにエフェクト登録
		/// </summary>
		/// <param name="pEffect">登録するエフェクトのポインタ</param>
		void AddXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// XAPOEffectの登録を空にする
		/// </summary>
		void ClearXAPOEffectList();

		/// <summary>
		/// AddXAPOEffectで登録したXAPOEffectを有効にする
		/// </summary>
		/// <param name="pEffect">AddXAPOEffectで登録したXAPOEffectのポインタ</param>
		void EnableXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// AddXAPOEffectで登録したXAPOEffectを無効にする
		/// </summary>
		/// <param name="pEffect">AddXAPOEffectで登録したXAPOEffectのポインタ</param>
		void DisableXAPOEffect(XAPOEffect* pEffect);

#pragma region Setter

		/// <summary>
		/// 音量をセットする
		/// </summary>
		/// <param name="volume">音量値(倍率指定)</param>
		void SetVolume(const float volume);

		/// <summary>
		/// ピッチをセットする
		/// </summary>
		/// <param name="pitch">ピッチ(倍率指定)</param>
		void SetPitch(const float pitch);

		/// <summary>
		/// パンをセットする
		/// </summary>
		/// <param name="pan">パン(-1～1)</param>
		/// <param name="pDstVoice">出力先をサブミックスボイスにしている場合、ポインタを指定する。</param>
		void SetPan(const float pan, SubmixVoice* pDstVoice = nullptr);

		/// <summary>
		/// パンをセットする
		/// </summary>
		/// <param name="dig">右側を0度、正面を90度としたときの角度</param>
		/// <param name="pDstVoice">出力先をサブミックスボイスにしている場合、ポインタを指定する。</param>
		void SetPanFromAngle(const float dig, SubmixVoice* pDstVoice = nullptr);

		/// <summary>
		/// ソースボイスに適用中のリバーブエフェクトにプリセットからパラメータをセット
		/// </summary>
		/// <param name="pEffect">適用するエフェクトのポインタ</param>
		/// <param name="preset">REVERB_PRESETから始まるプリセット定義 (DX12LIBSOUND_REBERB_PRESET_...)</param>
		void SetReverbParamsFromPreset(XAPOEffect* pEffect, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& preset);

		/// <summary>
		/// ソースボイスに適用中のリバーブエフェクトのパラメータをセット
		/// </summary>
		/// <param name="pEffect">適用するエフェクト(リバーブ)のポインタ</param>
		/// <param name="p">パラメータ配列のポインタ nullptrを渡すと初期設定になる</param>
		void SetReverbParams(XAPOEffect* pEffect, XAUDIO2FX_REVERB_PARAMETERS* p);

		/// <summary>
		/// 音源のループ設定
		/// </summary>
		/// <param name="isLoop">ループを有効にするか</param>
		/// <param name="loopCount">ループさせる回数 255で無限ループ</param>
		/// <param name="loopStartSec">ループ開始位置(ms)</param>
		/// <param name="loopEndSec">ループ終了位置(ms)</param>
		void SetLoopConfig(bool isLoop, const int loopCount = 255, const float loopStartSec = 0, const float loopEndSec = 0);

		/// <summary>
		/// フィルターエフェクトを設定する ※LoadWaveでisUseEffectをtrueにしたときのみ有効
		/// </summary>
		/// <param name="type">フィルターエフェクトの種類 (DX12LIBSOUND_EFFECT_FILTER_...)</param>
		/// <param name="frequency">ラジアン周波数</param>
		/// <param name="isSetAsHz">周波数をヘルツで指定するか</param>
		/// <param name="oneOverQ">Q値の逆数 ロー、ハイパスは1.4142fを指定するとよい</param>
		void SetFilterEffect(const XAUDIO2_FILTER_TYPE& type, const float frequency, const bool isSetAsHz = false, const float oneOverQ = 1.0f);

		/// <summary>
		/// X3DAudioで使用するエミッターの位置をセット
		/// </summary>
		/// <param name="x">X座標</param>
		/// <param name="y">Y座標</param>
		/// <param name="z">Z座標</param>
		void Set3DEmitterPos(const float x, const float y, const float z);

		/// <summary>
		/// X3DAudioで使用するエミッターの前方ベクトルをセット
		/// </summary>
		/// <param name="x">X要素</param>
		/// <param name="y">Y要素</param>
		/// <param name="z">Z要素</param>
		void Set3DEmitterFrontVec(const float x, const float y, const float z);

		/// <summary>
		/// X3DAudioで使用するエミッターの上方ベクトルをセット
		/// </summary>
		/// <param name="x">X要素</param>
		/// <param name="y">Y要素</param>
		/// <param name="z">Z要素</param>
		void Set3DEmitterUpVec(const float x, const float y, const float z);

		/// <summary>
		/// X3DAudioで使用するエミッターの音をどこまで響かせるかの倍率 デフォルトは14倍
		/// </summary>
		void Set3DEmitterDistanceScaler(const float scaler);

		/// <summary>
		/// X3DAudioで使用するエミッターのドップラー効果の影響度 デフォルトは1倍
		/// </summary>
		/// <param name="scaler"></param>
		void Set3DEmitterDoppelScaler(const float scaler);

		/// <summary>
		/// X3DAudioで使用するエミッターの内半径。デフォルトは2.0f
		/// </summary>
		/// <param name="r">半径</param>
		void Set3DEmitterInnerRadius(const float r);

		/// <summary>
		/// X3DAudioで使用するエミッターの出力角度。デフォルトは45度
		/// </summary>
		/// <param name="digrees"></param>
		void Set3DEmitterInnerAngle(const float digrees);

#pragma endregion

#pragma region Getter
		/// <summary>
		/// 現在の再生位置(ms)を取得
		/// </summary>
		/// <returns>現在の再生位置(ms)</returns>
		float GetPlayingPosition();

		/// <summary>
		/// 現在の再生音量を取得
		/// </summary>
		/// <returns>現在の再生音量(倍率)</returns>
		float GetVolume();

		/// <summary>
		/// ソースボイスのVOICE_DETAILSを返す
		/// </summary>
		/// <returns>VOICE_DETAILS</returns>
		XAUDIO2_VOICE_DETAILS GetVoiceDetalis() { return voiceDetails; }

		/// <summary>
		/// 再生中かどうかを取得
		/// </summary>
		/// <returns>再生中かどうか</returns>
		bool GetIsPlay();

		/// <summary>
		/// 一時停止中かどうかを取得
		/// </summary>
		/// <returns>一時停止中かどうか</returns>
		bool GetIsPause() { return isPause; }

		/// <summary>
		/// ループが有効になっているかを返す
		/// </summary>
		/// <returns></returns>
		bool GetIsLoop();

		float GetDoopel() { return dspSettings.DopplerFactor; }

#pragma endregion

	};

	class Sound
	{
		//エイリアス
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		template <typename T>
		using vector = std::vector<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

		//静的メンバ変数
	private:
		//X3DAudioインスタンス
		static X3DAUDIO_HANDLE x3DInstance;
		//リスナー
		static X3DAUDIO_LISTENER listener;
		//リスナーコーン
		static X3DAUDIO_CONE listenerCone;
		//前フレーム位置
		static XMFLOAT3 listenerPrevPos;

		//ソースボイス管理コンテナ
		static vector<SourceVoice*> sourceVoiceManager;
		//サブミックスボイス管理コンテナ
		static vector<SubmixVoice*> submixVoiceManager;
		//WaveData管理コンテナ
		static vector<WaveData*> waveDataManager;
		//XAPOEffect管理コンテナ
		static vector<XAPOEffect*> xapoEffectManager;

	private:

		//xAudio2インスタンス
		static IXAudio2* xAudio2;
		//マスターボイス
		static IXAudio2MasteringVoice* pMasterVoice;
		//デバイス情報
		static XAUDIO2_VOICE_DETAILS deviceDetails;
		//デバイスのチャンネルマスク
		static DWORD dwChannelMask;
		//3DAudioを使うか
		static bool isUse3DAudio;

		//静的メンバ関数
	public:
		/// <summary>
		/// XAudio2の初期化
		/// </summary>
		/// <param name="use3DAudio">3Dサウンド効果を使用するか</param>
		static void StaticInitialize(bool use3DAudio = false);

		/// <summary>
		/// 後始末 ソースボイスなどの処理もここで一括して行う
		/// </summary>
		static void StaticFinalize();

		/// <summary>
		/// ミリ秒をサンプル数に変換した数値を取得
		/// </summary>
		/// <param name="millisecond">変換するミリ秒</param>
		/// <param name="sampleRate">サンプリングレート</param>
		/// <returns>サンプル数</returns>
		static float ConvertMillisecondToSample(float millisecond, float sampleRate);

		/// <summary>
		/// X3DAudioで使用するリスナーの位置と角度をまとめて設定
		/// </summary>
		/// <param name="camera">カメラ</param>
		static void Set3DListenerPosAndVec(Camera& camera);

		/// <summary>
		/// X3DAudioで使用するリスナーの位置設定
		/// </summary>
		/// <param name="x">X座標</param>
		/// <param name="y">Y座標</param>
		/// <param name="z">Z座標</param>
		static void Set3DListenerPos(float x, float y, float z);

		/// <summary>
		/// X3DAudioで使用するリスナーの角度をカメラから計算
		/// </summary>
		/// <param name="camera">カメラ</param>
		static void Set3DListenerVec(Camera& camera);

		/// <summary>
		/// X3DAudioで使用するリスナーの前方角度設定
		/// </summary>
		/// <param name="x">X要素</param>
		/// <param name="y">Y要素</param>
		/// <param name="z">Z要素</param>
		static void Set3DListenerFrontVec(float x, float y, float z);

		/// <summary>
		/// X3DAudioで使用するリスナーの上方角度設定
		/// </summary>
		/// <param name="x">X要素</param>
		/// <param name="y">Y要素</param>
		/// <param name="z">Z要素</param>
		static void Set3DListenerUpVec(float x, float y, float z);

		/// <summary>
		/// XAudio2のインスタンス取得
		/// </summary>
		/// <returns></returns>
		static IXAudio2* GetXAudio2Instance() { return xAudio2; }

		/// <summary>
		/// DeviceDetails取得
		/// </summary>
		/// <returns></returns>
		static XAUDIO2_VOICE_DETAILS GetDeviceDetails() { return deviceDetails; }

		/// <summary>
		/// チャンネルマスク取得
		/// </summary>
		/// <returns></returns>
		static DWORD GetChannelMask() { return dwChannelMask; }

		/// <summary>
		/// マスターボイスのポインタ取得
		/// </summary>
		/// <returns></returns>
		static IXAudio2MasteringVoice* GetPMasterVoice() { return pMasterVoice; }

		/// <summary>
		/// 3Dオーディオを使用する設定か否か
		/// </summary>
		/// <returns></returns>
		static bool IsUse3DAudio() { return isUse3DAudio; }

		static X3DAUDIO_HANDLE* GetPX3DInstance() { return &x3DInstance; }

		/// <summary>
		/// 3Dオーディオで使用するリスナーのポインタ取得
		/// </summary>
		/// <returns></returns>
		static X3DAUDIO_LISTENER* GetPListener() { return &listener; }

		//以下オブジェクト管理用
		static void AddSourceVoice(SourceVoice* pSourceVoice);
		static void AddSubmixVoice(SubmixVoice* pSubmixVoice);
		static void AddWaveData(WaveData* pWaveData);
		static void AddXAPOEffect(XAPOEffect* pXAPOEffect);


	};

}