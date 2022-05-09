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
// �f�t�H���g(�Ӗ��Ȃ��H)
#define DX12LIBSOUND_REBERB_PRESET_DEFAULT XAUDIO2FX_I3DL2_PRESET_DEFAULT
// ��ʓI�ȋ��
#define DX12LIBSOUND_REBERB_PRESET_GENERIC XAUDIO2FX_I3DL2_PRESET_GENERIC
// ���_�a���Ҏ�
#define DX12LIBSOUND_REBERB_PRESET_PADDEDCELL XAUDIO2FX_I3DL2_PRESET_PADDEDCELL
// ����
#define DX12LIBSOUND_REBERB_PRESET_ROOM XAUDIO2FX_I3DL2_PRESET_ROOM
// �o�X���[��
#define DX12LIBSOUND_REBERB_PRESET_BATHROOM XAUDIO2FX_I3DL2_PRESET_BATHROOM
// ���r���O���[��
#define DX12LIBSOUND_REBERB_PRESET_LIVINGROOM XAUDIO2FX_I3DL2_PRESET_LIVINGROOM
// �΂̕���
#define DX12LIBSOUND_REBERB_PRESET_STONEROOM XAUDIO2FX_I3DL2_PRESET_STONEROOM
// �u��
#define DX12LIBSOUND_REBERB_PRESET_AUDITORIUM XAUDIO2FX_I3DL2_PRESET_AUDITORIUM
// �R���T�[�g�z�[��
#define DX12LIBSOUND_REBERB_PRESET_CONCERTHALL XAUDIO2FX_I3DL2_PRESET_CONCERTHALL
// ����
#define DX12LIBSOUND_REBERB_PRESET_CAVE XAUDIO2FX_I3DL2_PRESET_CAVE
// ����
#define DX12LIBSOUND_REBERB_PRESET_ARENA XAUDIO2FX_I3DL2_PRESET_ARENA
// �i�[��
#define DX12LIBSOUND_REBERB_PRESET_HANGAR XAUDIO2FX_I3DL2_PRESET_HANGAR
// �J�[�y�b�g���~���ꂽ����
#define DX12LIBSOUND_REBERB_PRESET_CARPETEDHALLWAY XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY
// ����
#define DX12LIBSOUND_REBERB_PRESET_HALLWAY XAUDIO2FX_I3DL2_PRESET_HALLWAY
// �΂̘L��
#define DX12LIBSOUND_REBERB_PRESET_STONECORRIDOR XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR
// ���ʂ�
#define DX12LIBSOUND_REBERB_PRESET_ALLEY XAUDIO2FX_I3DL2_PRESET_ALLEY
// �X
#define DX12LIBSOUND_REBERB_PRESET_FOREST XAUDIO2FX_I3DL2_PRESET_FOREST
// �s�s
#define DX12LIBSOUND_REBERB_PRESET_CITY XAUDIO2FX_I3DL2_PRESET_CITY
// �R
#define DX12LIBSOUND_REBERB_PRESET_MOUNTAINS XAUDIO2FX_I3DL2_PRESET_MOUNTAINS
// �̐Ώ�
#define DX12LIBSOUND_REBERB_PRESET_QUARRY XAUDIO2FX_I3DL2_PRESET_QUARRY
// ����
#define DX12LIBSOUND_REBERB_PRESET_PLAIN XAUDIO2FX_I3DL2_PRESET_PLAIN
// ���ԏ�
#define DX12LIBSOUND_REBERB_PRESET_PARKINGLOT XAUDIO2FX_I3DL2_PRESET_PARKINGLOT
// ������
#define DX12LIBSOUND_REBERB_PRESET_SEWERPIPE XAUDIO2FX_I3DL2_PRESET_SEWERPIPE
// ���ʉ�
#define DX12LIBSOUND_REBERB_PRESET_UNDERWATER XAUDIO2FX_I3DL2_PRESET_UNDERWATER
// ������
#define DX12LIBSOUND_REBERB_PRESET_SMALLROOM XAUDIO2FX_I3DL2_PRESET_SMALLROOM
// ������
#define DX12LIBSOUND_REBERB_PRESET_MEDIUMROOM XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM
// �啔��
#define DX12LIBSOUND_REBERB_PRESET_LARGEROOM XAUDIO2FX_I3DL2_PRESET_LARGEROOM
// ���z�[��
#define DX12LIBSOUND_REBERB_PRESET_MEDIUMHALL XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL
// ��z�[��
#define DX12LIBSOUND_REBERB_PRESET_LARGEHALL XAUDIO2FX_I3DL2_PRESET_LARGEHALL
// ��
#define DX12LIBSOUND_REBERB_PRESET_PLATE XAUDIO2FX_I3DL2_PRESET_PLATE
#pragma endregion

#pragma region Effect_Filter_Type

//���[�p�X�t�B���^
#define DX12LIBSOUND_EFFECT_FILTER_LOWPASS XAUDIO2_FILTER_TYPE::LowPassFilter
//�o���h�p�X�t�B���^
#define DX12LIBSOUND_EFFECT_FILTER_BANDPASS XAUDIO2_FILTER_TYPE::BandPassFilter
//�n�C�p�X�t�B���^
#define DX12LIBSOUND_EFFECT_FILTER_HIGHPASS XAUDIO2_FILTER_TYPE::HighPassFilter
//�m�b�`�t�B���^
#define DX12LIBSOUND_EFFECT_FILTER_NOTCHPASS XAUDIO2_FILTER_TYPE::NotchFilter
//�P�Ƀ��[�p�X�t�B���^
#define DX12LIBSOUND_EFFECT_FILTER_LOWPASS_ONEPOLE XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter
//�P�Ƀn�C�p�X�t�B���^
#define DX12LIBSOUND_EFFECT_FILTER_HIGHPASS_ONEPOLE XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter

#pragma endregion
#pragma endregion

namespace DX12Library {

	class XAudio2VoiceCallback;

	//�G�t�F�N�g�N���X
	class XAPOEffect
	{
		//�G�C���A�X
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

	private:
		//�f�X�N���v�^
		XAUDIO2_EFFECT_DESCRIPTOR effectDesc;
		//���o�[�u�G�t�F�N�g���ۂ�
		bool isReverb = false;

	public:
		~XAPOEffect();

		/// <summary>
		/// ���o�[�u�G�t�F�N�g����
		/// </summary>
		/// <param name="initialState">�����K�p���</param>
		void CreateReverb(bool initialState = true);

		/// <summary>
		/// XAPOEffect��j������
		/// </summary>
		void DestroyXAPOEffect();

		/// <summary>
		/// �f�X�N���v�^�擾
		/// </summary>
		/// <returns></returns>
		XAUDIO2_EFFECT_DESCRIPTOR GetEffectDescriptor() { return effectDesc; }

		/// <summary>
		/// ���o�[�u�G�t�F�N�g���ǂ�����Ԃ�
		/// </summary>
		/// <returns></returns>
		bool IsReverb() { return isReverb; }
	};

	//�T�u�~�b�N�X�{�C�X�N���X
	class SubmixVoice
	{
		//�G�C���A�X
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

	private:
		//�T�u�~�b�N�X�{�C�X
		IXAudio2SubmixVoice* pSubmixVoice;
		//�T�u�~�b�N�X�{�C�X�ɓK�p���̃G�t�F�N�g�̃L�[���i�[
		unordered_map<XAPOEffect*, int> effectIndexSubmix;

	public:
		~SubmixVoice();

		/// <summary>
		/// �T�u�~�b�N�X�{�C�X�𐶐�����
		/// </summary>
		/// <param name="channelCount">�`�����l����</param>
		/// <param name="samplingRate">�T���v�����O���[�g</param>
		void CreateSubmixVoice(const int channelCount, const float samplingRate);

		/// <summary>
		/// �T�u�~�b�N�X�{�C�X��j������
		/// </summary>
		void DestroySubmixVoice();

		/// <summary>
		/// �T�u�~�b�N�X�{�C�X�ɃG�t�F�N�g�o�^
		/// </summary>
		/// <param name="pEffect">�K�p����G�t�F�N�g�̃|�C���^</param>
		void AddXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// �G�t�F�N�g���X�g����ɂ���
		/// </summary>
		void ClearXAPOEffect();

		/// <summary>
		/// AddXAPOEffect�œo�^����XAPOEffect��L���ɂ���
		/// </summary>
		/// <param name="pEffect">AddXAPOEffect�œo�^����XAPOEffect�̃|�C���^</param>
		void EnableXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// AddXAPOEffect�œo�^����XAPOEffect�𖳌��ɂ���
		/// </summary>
		/// <param name="pEffect">AddXAPOEffect�œo�^����XAPOEffect�̃|�C���^</param>
		void DisableXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// �T�u�~�b�N�X�{�C�X�ɓK�p���̃��o�[�u�G�t�F�N�g�Ƀv���Z�b�g����p�����[�^���Z�b�g
		/// </summary>
		/// <param name="pEffect">�K�p����G�t�F�N�g�̃|�C���^</param>
		/// <param name="preset">REVERB_PRESET����n�܂�v���Z�b�g��` (DX12LIBSOUND_REBERB_PRESET_...)</param>
		void SetReverbParamsFromPreset(XAPOEffect* pEffect, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& preset);

		/// <summary>
		/// �T�u�~�b�N�X�{�C�X�ɓK�p���̃��o�[�u�G�t�F�N�g�̃p�����[�^���Z�b�g
		/// </summary>
		/// <param name="pEffect">�K�p����G�t�F�N�g�̃|�C���^</param>
		/// <param name="p">�p�����[�^�z��̃|�C���^ nullptr��n���Ə����ݒ�ɂȂ�</param>
		void SetReverbParams(XAPOEffect* pEffect, XAUDIO2FX_REVERB_PARAMETERS* p);

		/// <summary>
		/// ���ʂ��Z�b�g����
		/// </summary>
		/// <param name="volume">���ʒl(�{���w��)</param>
		void SetVolume(const float volume);

		/// <summary>
		/// �T�u�~�b�N�X�{�C�X�|�C���^�擾
		/// </summary>
		/// <returns></returns>
		IXAudio2SubmixVoice* GetPXAudio2SubmixVoice() { return pSubmixVoice; }

	};

	class WaveData
	{
		//�G�C���A�X
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

		//�����o�ϐ�
	private:
		//�t�H�[�}�b�g
		std::unique_ptr<WAVEFORMATEX> wfex;
		std::unique_ptr<char> soundBuffer;
		UINT32 audioBytes;

		//�����o�֐�
	public:
		~WaveData();

		/// <summary>
		/// wav�t�@�C�������[�h����
		/// </summary>
		/// <param name="filename">�t�@�C���p�X</param>
		void LoadWave(const std::wstring& filename);

		/// <summary>
		/// �����f�[�^���폜���� �������j����ɌĂяo���I�I
		/// </summary>
		void DeleteWave();

		/// <summary>
		/// �����̒������擾
		/// </summary>
		/// <returns>�����̒���(ms)</returns>
		float GetSoundLength();

		/// <summary>
		/// �����̃T���v�����O���[�g���擾
		/// </summary>
		/// <returns>�T���v�����O���[�g��</returns>
		float GetSamplingRate();

		/// <summary>
		/// �����̃`�����l�������擾
		/// </summary>
		/// <returns>�`�����l����</returns>
		int GetChannelCount();

		/// <summary>
		/// WAVEFORMATEX�擾
		/// </summary>
		/// <returns>WAVEFORMATEX</returns>
		WAVEFORMATEX* GetPWaveFormatEx() { return wfex.get(); }

		char* GetSoundBuffer() { return soundBuffer.get(); }

		UINT32 GetAudioBytes() { return audioBytes; }


	};

	//�\�[�X�{�C�X�N���X
	class SourceVoice
	{
		//�G�C���A�X
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

		//�����o�ϐ�
	private:

		//�\�[�X�{�C�X�̃|�C���^
		IXAudio2SourceVoice* pSourceVoice;
		//�R�[���o�b�N�I�u�W�F�N�g
		XAudio2VoiceCallback callback;
		//�����f�[�^�|�C���^
		WaveData* pWaveData;
		//�����|�C���^
		XAUDIO2_BUFFER buf = {};

		XAUDIO2_VOICE_DETAILS voiceDetails = {};

		//�G�t�F�N�g���g�p���邩
		bool isUseEffect = false;
		//�ő�s�b�`���[�g
		float maxPitchRate = 2.0f;
		//�ꎞ��~����
		bool isPause = false;

		//�s�b�`
		float pitch = 1.0f;

		//�o�^���̃G�t�F�N�g�̃L�[���i�[
		unordered_map<XAPOEffect*, int> effectIndex;

		//�o�͐�{�C�X���X�g
		std::vector<SubmixVoice*> submixVoiceList;


		//X3DAudio
		//Emitter
		X3DAUDIO_EMITTER emitter;
		X3DAUDIO_CONE emitterCone;
		X3DAUDIO_DSP_SETTINGS dspSettings;
		XMFLOAT3 emitterPrevPos;

		FLOAT32 azimuths[2];
		FLOAT32 matrix[2];

		//�����o�֐�
	public:
		~SourceVoice();

		/// <summary>
		/// �\�[�X�{�C�X�𐶐�����
		/// </summary>
		/// <param name="wavData">�����f�[�^</param>
		void CreateSourceVoice(WaveData* pWaveData, bool isUseEffect = false, const float maxPitchRate = 2.0f);

		/// <summary>
		/// �\�[�X�{�C�X��j������
		/// </summary>
		void DestroySourceVoice();

		/// <summary>
		/// �Đ�����
		/// </summary>
		void PlayWave();

		/// <summary>
		/// ��~����
		/// </summary>
		void StopWave();

		/// <summary>
		/// �ꎞ��~����
		/// </summary>
		void PauseWave();

		/// <summary>
		/// X3DAudio��Calculate�ƌv�Z���ʓK�p
		/// </summary>
		void Update3DAudio();

		/// <summary>
		/// �t�B���^�[�G�t�F�N�g�𖳌�������
		/// </summary>
		void DisableFilterEffect();

		/// <summary>
		/// �o�͐�T�u�~�b�N�X�{�C�X��ǉ�
		/// </summary>
		/// <param name="pSubmixVoice">���������T�u�~�b�N�X�{�C�X�̃|�C���^</param>
		void AddOutputSubmixVoice(SubmixVoice* pSubmixVoice);

		/// <summary>
		/// �o�͐�T�u�~�b�N�X�{�C�X���N���A
		/// </summary>
		void ClearOutputSubmixVoice();

		/// <summary>
		/// �\�[�X�{�C�X�ɃG�t�F�N�g�o�^
		/// </summary>
		/// <param name="pEffect">�o�^����G�t�F�N�g�̃|�C���^</param>
		void AddXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// XAPOEffect�̓o�^����ɂ���
		/// </summary>
		void ClearXAPOEffectList();

		/// <summary>
		/// AddXAPOEffect�œo�^����XAPOEffect��L���ɂ���
		/// </summary>
		/// <param name="pEffect">AddXAPOEffect�œo�^����XAPOEffect�̃|�C���^</param>
		void EnableXAPOEffect(XAPOEffect* pEffect);

		/// <summary>
		/// AddXAPOEffect�œo�^����XAPOEffect�𖳌��ɂ���
		/// </summary>
		/// <param name="pEffect">AddXAPOEffect�œo�^����XAPOEffect�̃|�C���^</param>
		void DisableXAPOEffect(XAPOEffect* pEffect);

#pragma region Setter

		/// <summary>
		/// ���ʂ��Z�b�g����
		/// </summary>
		/// <param name="volume">���ʒl(�{���w��)</param>
		void SetVolume(const float volume);

		/// <summary>
		/// �s�b�`���Z�b�g����
		/// </summary>
		/// <param name="pitch">�s�b�`(�{���w��)</param>
		void SetPitch(const float pitch);

		/// <summary>
		/// �p�����Z�b�g����
		/// </summary>
		/// <param name="pan">�p��(-1�`1)</param>
		/// <param name="pDstVoice">�o�͐���T�u�~�b�N�X�{�C�X�ɂ��Ă���ꍇ�A�|�C���^���w�肷��B</param>
		void SetPan(const float pan, SubmixVoice* pDstVoice = nullptr);

		/// <summary>
		/// �p�����Z�b�g����
		/// </summary>
		/// <param name="dig">�E����0�x�A���ʂ�90�x�Ƃ����Ƃ��̊p�x</param>
		/// <param name="pDstVoice">�o�͐���T�u�~�b�N�X�{�C�X�ɂ��Ă���ꍇ�A�|�C���^���w�肷��B</param>
		void SetPanFromAngle(const float dig, SubmixVoice* pDstVoice = nullptr);

		/// <summary>
		/// �\�[�X�{�C�X�ɓK�p���̃��o�[�u�G�t�F�N�g�Ƀv���Z�b�g����p�����[�^���Z�b�g
		/// </summary>
		/// <param name="pEffect">�K�p����G�t�F�N�g�̃|�C���^</param>
		/// <param name="preset">REVERB_PRESET����n�܂�v���Z�b�g��` (DX12LIBSOUND_REBERB_PRESET_...)</param>
		void SetReverbParamsFromPreset(XAPOEffect* pEffect, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& preset);

		/// <summary>
		/// �\�[�X�{�C�X�ɓK�p���̃��o�[�u�G�t�F�N�g�̃p�����[�^���Z�b�g
		/// </summary>
		/// <param name="pEffect">�K�p����G�t�F�N�g(���o�[�u)�̃|�C���^</param>
		/// <param name="p">�p�����[�^�z��̃|�C���^ nullptr��n���Ə����ݒ�ɂȂ�</param>
		void SetReverbParams(XAPOEffect* pEffect, XAUDIO2FX_REVERB_PARAMETERS* p);

		/// <summary>
		/// �����̃��[�v�ݒ�
		/// </summary>
		/// <param name="isLoop">���[�v��L���ɂ��邩</param>
		/// <param name="loopCount">���[�v������� 255�Ŗ������[�v</param>
		/// <param name="loopStartSec">���[�v�J�n�ʒu(ms)</param>
		/// <param name="loopEndSec">���[�v�I���ʒu(ms)</param>
		void SetLoopConfig(bool isLoop, const int loopCount = 255, const float loopStartSec = 0, const float loopEndSec = 0);

		/// <summary>
		/// �t�B���^�[�G�t�F�N�g��ݒ肷�� ��LoadWave��isUseEffect��true�ɂ����Ƃ��̂ݗL��
		/// </summary>
		/// <param name="type">�t�B���^�[�G�t�F�N�g�̎�� (DX12LIBSOUND_EFFECT_FILTER_...)</param>
		/// <param name="frequency">���W�A�����g��</param>
		/// <param name="isSetAsHz">���g�����w���c�Ŏw�肷�邩</param>
		/// <param name="oneOverQ">Q�l�̋t�� ���[�A�n�C�p�X��1.4142f���w�肷��Ƃ悢</param>
		void SetFilterEffect(const XAUDIO2_FILTER_TYPE& type, const float frequency, const bool isSetAsHz = false, const float oneOverQ = 1.0f);

		/// <summary>
		/// X3DAudio�Ŏg�p����G�~�b�^�[�̈ʒu���Z�b�g
		/// </summary>
		/// <param name="x">X���W</param>
		/// <param name="y">Y���W</param>
		/// <param name="z">Z���W</param>
		void Set3DEmitterPos(const float x, const float y, const float z);

		/// <summary>
		/// X3DAudio�Ŏg�p����G�~�b�^�[�̑O���x�N�g�����Z�b�g
		/// </summary>
		/// <param name="x">X�v�f</param>
		/// <param name="y">Y�v�f</param>
		/// <param name="z">Z�v�f</param>
		void Set3DEmitterFrontVec(const float x, const float y, const float z);

		/// <summary>
		/// X3DAudio�Ŏg�p����G�~�b�^�[�̏���x�N�g�����Z�b�g
		/// </summary>
		/// <param name="x">X�v�f</param>
		/// <param name="y">Y�v�f</param>
		/// <param name="z">Z�v�f</param>
		void Set3DEmitterUpVec(const float x, const float y, const float z);

		/// <summary>
		/// X3DAudio�Ŏg�p����G�~�b�^�[�̉����ǂ��܂ŋ������邩�̔{�� �f�t�H���g��14�{
		/// </summary>
		void Set3DEmitterDistanceScaler(const float scaler);

		/// <summary>
		/// X3DAudio�Ŏg�p����G�~�b�^�[�̃h�b�v���[���ʂ̉e���x �f�t�H���g��1�{
		/// </summary>
		/// <param name="scaler"></param>
		void Set3DEmitterDoppelScaler(const float scaler);

		/// <summary>
		/// X3DAudio�Ŏg�p����G�~�b�^�[�̓����a�B�f�t�H���g��2.0f
		/// </summary>
		/// <param name="r">���a</param>
		void Set3DEmitterInnerRadius(const float r);

		/// <summary>
		/// X3DAudio�Ŏg�p����G�~�b�^�[�̏o�͊p�x�B�f�t�H���g��45�x
		/// </summary>
		/// <param name="digrees"></param>
		void Set3DEmitterInnerAngle(const float digrees);

#pragma endregion

#pragma region Getter
		/// <summary>
		/// ���݂̍Đ��ʒu(ms)���擾
		/// </summary>
		/// <returns>���݂̍Đ��ʒu(ms)</returns>
		float GetPlayingPosition();

		/// <summary>
		/// ���݂̍Đ����ʂ��擾
		/// </summary>
		/// <returns>���݂̍Đ�����(�{��)</returns>
		float GetVolume();

		/// <summary>
		/// �\�[�X�{�C�X��VOICE_DETAILS��Ԃ�
		/// </summary>
		/// <returns>VOICE_DETAILS</returns>
		XAUDIO2_VOICE_DETAILS GetVoiceDetalis() { return voiceDetails; }

		/// <summary>
		/// �Đ������ǂ������擾
		/// </summary>
		/// <returns>�Đ������ǂ���</returns>
		bool GetIsPlay();

		/// <summary>
		/// �ꎞ��~�����ǂ������擾
		/// </summary>
		/// <returns>�ꎞ��~�����ǂ���</returns>
		bool GetIsPause() { return isPause; }

		/// <summary>
		/// ���[�v���L���ɂȂ��Ă��邩��Ԃ�
		/// </summary>
		/// <returns></returns>
		bool GetIsLoop();

		float GetDoopel() { return dspSettings.DopplerFactor; }

#pragma endregion

	};

	class Sound
	{
		//�G�C���A�X
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		template <typename T>
		using vector = std::vector<T>;
		using string = std::string;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		template <typename T, typename U>
		using unordered_map = std::unordered_map<T, U>;

		//�ÓI�����o�ϐ�
	private:
		//X3DAudio�C���X�^���X
		static X3DAUDIO_HANDLE x3DInstance;
		//���X�i�[
		static X3DAUDIO_LISTENER listener;
		//���X�i�[�R�[��
		static X3DAUDIO_CONE listenerCone;
		//�O�t���[���ʒu
		static XMFLOAT3 listenerPrevPos;

		//�\�[�X�{�C�X�Ǘ��R���e�i
		static vector<SourceVoice*> sourceVoiceManager;
		//�T�u�~�b�N�X�{�C�X�Ǘ��R���e�i
		static vector<SubmixVoice*> submixVoiceManager;
		//WaveData�Ǘ��R���e�i
		static vector<WaveData*> waveDataManager;
		//XAPOEffect�Ǘ��R���e�i
		static vector<XAPOEffect*> xapoEffectManager;

	private:

		//xAudio2�C���X�^���X
		static IXAudio2* xAudio2;
		//�}�X�^�[�{�C�X
		static IXAudio2MasteringVoice* pMasterVoice;
		//�f�o�C�X���
		static XAUDIO2_VOICE_DETAILS deviceDetails;
		//�f�o�C�X�̃`�����l���}�X�N
		static DWORD dwChannelMask;
		//3DAudio���g����
		static bool isUse3DAudio;

		//�ÓI�����o�֐�
	public:
		/// <summary>
		/// XAudio2�̏�����
		/// </summary>
		/// <param name="use3DAudio">3D�T�E���h���ʂ��g�p���邩</param>
		static void StaticInitialize(bool use3DAudio = false);

		/// <summary>
		/// ��n�� �\�[�X�{�C�X�Ȃǂ̏����������ňꊇ���čs��
		/// </summary>
		static void StaticFinalize();

		/// <summary>
		/// �~���b���T���v�����ɕϊ��������l���擾
		/// </summary>
		/// <param name="millisecond">�ϊ�����~���b</param>
		/// <param name="sampleRate">�T���v�����O���[�g</param>
		/// <returns>�T���v����</returns>
		static float ConvertMillisecondToSample(float millisecond, float sampleRate);

		/// <summary>
		/// X3DAudio�Ŏg�p���郊�X�i�[�̈ʒu�Ɗp�x���܂Ƃ߂Đݒ�
		/// </summary>
		/// <param name="camera">�J����</param>
		static void Set3DListenerPosAndVec(Camera& camera);

		/// <summary>
		/// X3DAudio�Ŏg�p���郊�X�i�[�̈ʒu�ݒ�
		/// </summary>
		/// <param name="x">X���W</param>
		/// <param name="y">Y���W</param>
		/// <param name="z">Z���W</param>
		static void Set3DListenerPos(float x, float y, float z);

		/// <summary>
		/// X3DAudio�Ŏg�p���郊�X�i�[�̊p�x���J��������v�Z
		/// </summary>
		/// <param name="camera">�J����</param>
		static void Set3DListenerVec(Camera& camera);

		/// <summary>
		/// X3DAudio�Ŏg�p���郊�X�i�[�̑O���p�x�ݒ�
		/// </summary>
		/// <param name="x">X�v�f</param>
		/// <param name="y">Y�v�f</param>
		/// <param name="z">Z�v�f</param>
		static void Set3DListenerFrontVec(float x, float y, float z);

		/// <summary>
		/// X3DAudio�Ŏg�p���郊�X�i�[�̏���p�x�ݒ�
		/// </summary>
		/// <param name="x">X�v�f</param>
		/// <param name="y">Y�v�f</param>
		/// <param name="z">Z�v�f</param>
		static void Set3DListenerUpVec(float x, float y, float z);

		/// <summary>
		/// XAudio2�̃C���X�^���X�擾
		/// </summary>
		/// <returns></returns>
		static IXAudio2* GetXAudio2Instance() { return xAudio2; }

		/// <summary>
		/// DeviceDetails�擾
		/// </summary>
		/// <returns></returns>
		static XAUDIO2_VOICE_DETAILS GetDeviceDetails() { return deviceDetails; }

		/// <summary>
		/// �`�����l���}�X�N�擾
		/// </summary>
		/// <returns></returns>
		static DWORD GetChannelMask() { return dwChannelMask; }

		/// <summary>
		/// �}�X�^�[�{�C�X�̃|�C���^�擾
		/// </summary>
		/// <returns></returns>
		static IXAudio2MasteringVoice* GetPMasterVoice() { return pMasterVoice; }

		/// <summary>
		/// 3D�I�[�f�B�I���g�p����ݒ肩�ۂ�
		/// </summary>
		/// <returns></returns>
		static bool IsUse3DAudio() { return isUse3DAudio; }

		static X3DAUDIO_HANDLE* GetPX3DInstance() { return &x3DInstance; }

		/// <summary>
		/// 3D�I�[�f�B�I�Ŏg�p���郊�X�i�[�̃|�C���^�擾
		/// </summary>
		/// <returns></returns>
		static X3DAUDIO_LISTENER* GetPListener() { return &listener; }

		//�ȉ��I�u�W�F�N�g�Ǘ��p
		static void AddSourceVoice(SourceVoice* pSourceVoice);
		static void AddSubmixVoice(SubmixVoice* pSubmixVoice);
		static void AddWaveData(WaveData* pWaveData);
		static void AddXAPOEffect(XAPOEffect* pXAPOEffect);


	};

}