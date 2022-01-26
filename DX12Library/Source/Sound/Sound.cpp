#include "Sound.h"
#include <fstream>
#include <cassert>
#include <mmsystem.h>
#include "Quaternion.h"
#include "Vector3.h"

#pragma comment ( lib, "winmm.lib" )

//�ÓI�����o�ϐ��̎���
IXAudio2* Sound::xAudio2;
IXAudio2MasteringVoice* Sound::pMasterVoice;
XAUDIO2_VOICE_DETAILS Sound::deviceDetails;
DWORD Sound::dwChannelMask;
X3DAUDIO_HANDLE Sound::x3DInstance;
bool Sound::isUse3DAudio = false;
X3DAUDIO_LISTENER Sound::listener;
DirectX::XMFLOAT3 Sound::listenerPrevPos;
//from Sample
X3DAUDIO_CONE Sound::listenerCone = { X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };
std::vector<SourceVoice*> Sound::sourceVoiceManager;
std::vector<SubmixVoice*> Sound::submixVoiceManager;
std::vector<WaveData*> Sound::waveDataManager;
std::vector<XAPOEffect*> Sound::xapoEffectManager;

void Sound::StaticInitialize(bool use3DAudio)
{
	HRESULT result;

	//COM�̏�����
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		assert(0);
		//���
		CoUninitialize();
		return;
	}

	//XAudio�G���W���̃C���X�^���X�𐶐�
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result))
	{
		assert(0);
		//���
		CoUninitialize();
		return;
	}

	//�}�X�^�[�{�C�X�𐶐�
	result = xAudio2->CreateMasteringVoice(&pMasterVoice);
	if (FAILED(result))
	{
		assert(0);
		StaticFinalize();
		return;
	}

	//�`�����l���}�X�N�擾
	result = pMasterVoice->GetChannelMask(&dwChannelMask);
	if (FAILED(result))
	{
		assert(0);
		StaticFinalize();
		return;
	}

	//�f�o�C�X�f�[�^�擾
	pMasterVoice->GetVoiceDetails(&deviceDetails);

	if (use3DAudio) {
		//X3DAudio������
		X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance);

		//���X�i�[������
		listener.Position = { 0,0,0 };
		listenerPrevPos = { 0,0,0 };
		listener.OrientFront.x = 0;
		listener.OrientFront.y = 0;
		listener.OrientFront.z = 1;
		listener.OrientTop.x = 0;
		listener.OrientTop.y = 1;
		listener.OrientTop.z = 0;

		listener.pCone = &listenerCone;

	}

	isUse3DAudio = use3DAudio;
}

void Sound::StaticFinalize()
{
	//�\�[�X�{�C�X��n��
	for (int i = 0; i < sourceVoiceManager.size(); i++) {
		sourceVoiceManager[i]->DestroySourceVoice();
	}
	//�T�u�~�b�N�X�{�C�X��n��
	for (int i = 0; i < submixVoiceManager.size(); i++) {
		submixVoiceManager[i]->DestroySubmixVoice();
	}
	//XAPOEffect��n��
	for (int i = 0; i < xapoEffectManager.size(); i++) {
		xapoEffectManager[i]->DestroyXAPOEffect();
	}
	//WaveData��n��
	for (int i = 0; i < waveDataManager.size(); i++) {
		waveDataManager[i]->DeleteWave();
	}

	//�}�X�^�[�{�C�X���
	if (pMasterVoice)
	{
		pMasterVoice->DestroyVoice();
		pMasterVoice = nullptr;
	}

	if (xAudio2)
	{
		xAudio2->Release();
		xAudio2 = nullptr;
	}

	CoUninitialize();
}

float Sound::ConvertMillisecondToSample(float millisecond, float sampleRate)
{
	return sampleRate * millisecond * 0.001f;
}

void Sound::Set3DListenerPosAndVec(Camera& camera)
{
	Set3DListenerPos(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
	Set3DListenerVec(camera);
}

void Sound::Set3DListenerPos(float x, float y, float z)
{
	listener.Position.x = x;
	listener.Position.y = y;
	listener.Position.z = z;

	listener.Velocity.x = (listener.Position.x - listenerPrevPos.x);
	listener.Velocity.y = (listener.Position.y - listenerPrevPos.y);
	listener.Velocity.z = (listener.Position.z - listenerPrevPos.z);

	listenerPrevPos.x = listener.Position.x;
	listenerPrevPos.y = listener.Position.y;
	listenerPrevPos.z = listener.Position.z;
}

void Sound::Set3DListenerVec(Camera& camera)
{
	Vector3 front = {
		camera.GetTarget().x - camera.GetPosition().x,
		camera.GetTarget().y - camera.GetPosition().y,
		camera.GetTarget().z - camera.GetPosition().z
	};

	Vector3 up = { camera.GetUpVec().x, camera.GetUpVec().y, camera.GetUpVec().z };

	//���K��
	front = front.Normalize();

	//���K��
	up = up.Normalize();

	//��]�s��ŉ�]
	Quaternion rot(camera.GetRotMatrix());
	Quaternion rotFront(front, rot);
	Quaternion rotUp(up, rot);

	Set3DListenerFrontVec(rotFront.x, rotFront.y, rotFront.z);
	Set3DListenerUpVec(rotUp.x, rotUp.y, rotUp.z);
}

void Sound::Set3DListenerFrontVec(float x, float y, float z)
{
	listener.OrientFront.x = x;
	listener.OrientFront.y = y;
	listener.OrientFront.z = z;
}

void Sound::Set3DListenerUpVec(float x, float y, float z)
{
	listener.OrientTop.x = x;
	listener.OrientTop.y = y;
	listener.OrientTop.z = z;
}

void Sound::AddSourceVoice(SourceVoice* pSourceVoice)
{
	//����v�f�����݂��Ă��Ȃ�������ǉ�
	if (std::find(sourceVoiceManager.begin(), sourceVoiceManager.end(), pSourceVoice) == sourceVoiceManager.end()) {
		sourceVoiceManager.emplace_back(pSourceVoice);
	}
}

void Sound::AddSubmixVoice(SubmixVoice* pSubmixVoice)
{
	//����v�f�����݂��Ă��Ȃ�������ǉ�
	if (std::find(submixVoiceManager.begin(), submixVoiceManager.end(), pSubmixVoice) == submixVoiceManager.end()) {
		submixVoiceManager.emplace_back(pSubmixVoice);
	}
}

void Sound::AddWaveData(WaveData* pWaveData)
{
	//����v�f�����݂��Ă��Ȃ�������ǉ�
	if (std::find(waveDataManager.begin(), waveDataManager.end(), pWaveData) == waveDataManager.end()) {
		waveDataManager.emplace_back(pWaveData);
	}
}

void Sound::AddXAPOEffect(XAPOEffect* pXAPOEffect)
{
	//����v�f�����݂��Ă��Ȃ�������ǉ�
	if (std::find(xapoEffectManager.begin(), xapoEffectManager.end(), pXAPOEffect) == xapoEffectManager.end()) {
		xapoEffectManager.emplace_back(pXAPOEffect);
	}
}

XAPOEffect::~XAPOEffect()
{
	DestroyXAPOEffect();
}

void XAPOEffect::CreateReverb(bool initialState)
{
	//XAPO����
	IUnknown* pXAPO;
	XAudio2CreateReverb(&pXAPO);

	//�f�X�N���v�^�̍쐬
	effectDesc.InitialState = initialState;
	effectDesc.OutputChannels = 2; //�Ƃ肠����2ch���ߑł�
	effectDesc.pEffect = pXAPO;

	isReverb = true;

	Sound::AddXAPOEffect(this);
}

void XAPOEffect::DestroyXAPOEffect()
{
	if (effectDesc.pEffect) {
		effectDesc.pEffect->Release();
	}
}

SubmixVoice::~SubmixVoice()
{
	DestroySubmixVoice();
}

void SubmixVoice::CreateSubmixVoice(const int channelCount, const float samplingRate)
{
	//�����̏������ƂɃT�u�~�b�N�X�{�C�X����
	HRESULT result = Sound::GetXAudio2Instance()->CreateSubmixVoice(&pSubmixVoice, (UINT32)channelCount, (UINT32)samplingRate);
	
	Sound::AddSubmixVoice(this);
}

void SubmixVoice::DestroySubmixVoice()
{
	//�j��
	if (pSubmixVoice) {
		ClearXAPOEffect();

		pSubmixVoice->DestroyVoice();
		pSubmixVoice = nullptr;
	}
}

void SubmixVoice::AddXAPOEffect(XAPOEffect* pEffect)
{
	std::vector<XAUDIO2_EFFECT_DESCRIPTOR> desc;

	//�G�t�F�N�g���X�g�ɓo�^
	effectIndexSubmix.emplace(pEffect, effectIndexSubmix.size());

	for (auto&& v : effectIndexSubmix) {
		desc.push_back(v.first->GetEffectDescriptor());
	}

	//EffectChain�̍쐬
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = desc.size(); //������
	chain.pEffectDescriptors = desc.data();

	// �{�C�X�� EFFECT_CHAIN ��}��
	HRESULT result = pSubmixVoice->SetEffectChain(&chain);

	desc.clear();
}

void SubmixVoice::ClearXAPOEffect()
{
	effectIndexSubmix.clear();

	//EffectChain�̍쐬
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 0;
	chain.pEffectDescriptors = nullptr;

	// �{�C�X�� EFFECT_CHAIN ��}��
	HRESULT result = pSubmixVoice->SetEffectChain(&chain);

}

void SubmixVoice::EnableXAPOEffect(XAPOEffect* pEffect)
{
	//�o�^����Ă��Ȃ������烊�^�[��
	if (effectIndexSubmix.find(pEffect) == effectIndexSubmix.end()) {
		return;
	}

	HRESULT result = pSubmixVoice->EnableEffect(effectIndexSubmix[pEffect]);
}

void SubmixVoice::DisableXAPOEffect(XAPOEffect* pEffect)
{
	//�o�^����Ă��Ȃ������烊�^�[��
	if (effectIndexSubmix.find(pEffect) == effectIndexSubmix.end()) {
		return;
	}

	HRESULT result = pSubmixVoice->DisableEffect(effectIndexSubmix[pEffect]);
}

void SubmixVoice::SetReverbParamsFromPreset(XAPOEffect* pEffect, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& preset)
{
	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	ReverbConvertI3DL2ToNative(&preset, &reverbParameters);

	HRESULT result = pSubmixVoice->SetEffectParameters(effectIndexSubmix[pEffect], &reverbParameters, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
}

void SubmixVoice::SetReverbParams(XAPOEffect* pEffect, XAUDIO2FX_REVERB_PARAMETERS* p)
{
	if (p == nullptr) {
		XAUDIO2FX_REVERB_PARAMETERS params;

		params.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
		params.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
		params.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
		params.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		params.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		params.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		params.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		params.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
		params.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
		params.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
		params.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
		params.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
		params.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
		params.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
		params.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
		params.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
		params.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
		params.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
		params.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
		params.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
		params.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
		params.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;

		HRESULT result = pSubmixVoice->SetEffectParameters(effectIndexSubmix[pEffect], &params, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
	}
	else {
		HRESULT result = pSubmixVoice->SetEffectParameters(effectIndexSubmix[pEffect], p, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
	}
}

void SubmixVoice::SetVolume(const float volume)
{
	HRESULT result = pSubmixVoice->SetVolume(volume);
}

WaveData::~WaveData()
{
	DeleteWave();
}

void WaveData::LoadWave(const wchar_t* filename)
{
	HRESULT result;
	MMRESULT mmResult;

	//wav�t�@�C�����J��
	HMMIO mmioHandle = mmioOpen((LPWSTR)filename, NULL, MMIO_READ);
	if (mmioHandle == NULL) {
		assert(0);
	}

	MMCKINFO riffinfo;
	riffinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	//RIFF�`�����N�ɐi��
	mmResult = mmioDescend(mmioHandle, &riffinfo, NULL, MMIO_FINDRIFF);
	if (mmResult != NOERROR) {
		assert(0);
		//�N���[�Y
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//fmt�`�����N
	MMCKINFO ckinfo;
	ckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmResult = mmioDescend(mmioHandle, &ckinfo, &riffinfo, MMIO_FINDCHUNK);
	if (mmResult != NOERROR) {
		assert(0);
		//�N���[�Y
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//PCMWAVEFORMAT�擾
	PCMWAVEFORMAT pwf;
	LONG size = mmioRead(mmioHandle, (HPSTR)&pwf, sizeof(pwf));
	//�ǂݍ��݃T�C�Y�����������`�F�b�N
	if (size != sizeof(pwf)) {
		assert(0);
		//�N���[�Y
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//�]����BYTE�����邩
	if (pwf.wf.wFormatTag == WAVE_FORMAT_PCM) {
		wfex = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX)];
		if (wfex == NULL) {
			assert(0);
			//�N���[�Y
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}

		//WAVEFORMATEX�ɃR�s�[
		memcpy(wfex, &pwf, sizeof(pwf));
		wfex->cbSize = 0;
	}
	else {
		//�]����BYTE�ǂݍ���
		WORD cbExtraBytes = 0L;
		size = mmioRead(mmioHandle, (CHAR*)&cbExtraBytes, sizeof(WORD));
		if (size != sizeof(WORD)) {
			assert(0);
			//�N���[�Y
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}

		wfex = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX) + cbExtraBytes];
		if (wfex == NULL) {
			assert(0);
			//�N���[�Y
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}

		//WAVEFORMATEX�ɃR�s�[
		memcpy(wfex, &pwf, sizeof(pwf));
		wfex->cbSize = cbExtraBytes;

		//�]����BYTE�ǂݍ���
		size = mmioRead(mmioHandle, (CHAR*)(((BYTE*)&(wfex->cbSize)) + sizeof(WORD)), cbExtraBytes);
		if (size != cbExtraBytes) {
			assert(0);
			//�N���[�Y
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}
	}

	//�ǂݍ��߂���fmt�`�����N�𔲂���
	mmResult = mmioAscend(mmioHandle, &ckinfo, 0);
	if (mmResult != MMSYSERR_NOERROR) {
		assert(0);
		//�N���[�Y
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//data�`�����N
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmResult = mmioDescend(mmioHandle, &ckinfo, &riffinfo, MMIO_FINDCHUNK);
	if (mmResult != NOERROR) {
		assert(0);
		//�N���[�Y
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//���f�[�^�̓ǂݍ���
	soundBuffer = new char[ckinfo.cksize];
	size = mmioRead(mmioHandle, (HPSTR)soundBuffer, ckinfo.cksize);
	if (size != ckinfo.cksize) {
		delete[] soundBuffer;
		assert(0);
		//�N���[�Y
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//�N���[�Y
	mmioClose(mmioHandle, MMIO_FHOPEN);
	mmioHandle = NULL;

	audioBytes = ckinfo.cksize;

	Sound::AddWaveData(this);

}

void WaveData::DeleteWave()
{
	if (wfex) {
		delete[] wfex;
		wfex = nullptr;
	}

	if (soundBuffer) {
		delete[] soundBuffer;
		soundBuffer = nullptr;
	}
}

float WaveData::GetSoundLength()
{
	return (float)audioBytes / wfex->nAvgBytesPerSec * 1000;
}

float WaveData::GetSamplingRate()
{
	return (float)wfex->nSamplesPerSec;
}

int WaveData::GetChannelCount()
{
	return (int)wfex->nChannels;
}

SourceVoice::~SourceVoice()
{
	DestroySourceVoice();
}

void SourceVoice::CreateSourceVoice(WaveData* pWaveData, bool isUseEffect, const float maxPitchRate)
{
	HRESULT result = S_FALSE;

	this->pWaveData = pWaveData;

	//�N���b�s���O
	if (maxPitchRate > XAUDIO2_MAX_FREQ_RATIO) {
		this->maxPitchRate = XAUDIO2_MAX_FREQ_RATIO;
	}
	else if (maxPitchRate < 1.0f) {
		this->maxPitchRate = 1.0f;
	}
	else {
		this->maxPitchRate = maxPitchRate;
	}

	//�g�`�t�H�[�}�b�g������SourceVoice�̐���
	if (isUseEffect) {
		//�G�t�F�N�g�g�p�ݒ�
		result = Sound::GetXAudio2Instance()->CreateSourceVoice(&pSourceVoice, this->pWaveData->GetPWaveFormatEx(), XAUDIO2_VOICE_USEFILTER, this->maxPitchRate, &callback);
	}
	else {
		//�G�t�F�N�g���g��Ȃ��ݒ�
		result = Sound::GetXAudio2Instance()->CreateSourceVoice(&pSourceVoice, this->pWaveData->GetPWaveFormatEx(), 0, this->maxPitchRate, &callback);
	}
	this->isUseEffect = isUseEffect;

	//�Đ�����g�`�f�[�^�̐ݒ�
	buf.pAudioData = (BYTE*)pWaveData->GetSoundBuffer();
	buf.pContext = pWaveData->GetSoundBuffer();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = pWaveData->GetAudioBytes();

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	pSourceVoice->GetVoiceDetails(&voiceDetails);

	submixVoiceList.clear();

	if (Sound::IsUse3DAudio()) {
		emitter.pCone = &emitterCone;
		emitter.pCone->InnerAngle = 0.0f;
		emitter.pCone->OuterAngle = 0.0f;
		emitter.pCone->InnerVolume = 0.0f;
		emitter.pCone->OuterVolume = 1.0f;
		emitter.pCone->InnerLPF = 0.0f;
		emitter.pCone->OuterLPF = 1.0f;
		emitter.pCone->InnerReverb = 0.0f;
		emitter.pCone->OuterReverb = 1.0f;

		emitter.Position.x = 0;
		emitter.Position.y = 0;
		emitter.Position.z = 0;

		emitter.OrientFront.x = 0;
		emitter.OrientFront.y = 0;
		emitter.OrientFront.z = 1;

		emitter.OrientTop.x = 0;
		emitter.OrientTop.y = 1;
		emitter.OrientTop.z = 0;


		emitter.ChannelCount = voiceDetails.InputChannels;
		emitter.ChannelRadius = 1.0f;

		emitter.InnerRadius = 2.0f;
		emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
		azimuths = new float[Sound::GetDeviceDetails().InputChannels];
		emitter.pChannelAzimuths = azimuths;

		static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
		static const X3DAUDIO_DISTANCE_CURVE       Emitter_LFE_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_LFE_CurvePoints[0], 3 };

		emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
		emitter.pLFECurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
		emitter.pLPFDirectCurve = nullptr;
		emitter.pLPFReverbCurve = nullptr;

		static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
		static const X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0], 3 };

		emitter.pReverbCurve = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
		emitter.CurveDistanceScaler = 14.0f;
		emitter.DopplerScaler = 1.0f;

		//DSP
		matrix = new float[Sound::GetDeviceDetails().InputChannels];
		dspSettings.SrcChannelCount = voiceDetails.InputChannels;
		dspSettings.DstChannelCount = Sound::GetDeviceDetails().InputChannels;
		dspSettings.pMatrixCoefficients = matrix;

	}

	Sound::AddSourceVoice(this);
}

void SourceVoice::DestroySourceVoice()
{
	if (pSourceVoice) {
		StopWave();
		ClearOutputSubmixVoice();
		ClearXAPOEffectList();

		pSourceVoice->DestroyVoice();
		pSourceVoice = nullptr;
	}
}

void SourceVoice::PlayWave()
{
	HRESULT result = S_FALSE;

	if (GetIsPlay()) {
		StopWave();
	}

	//�g�`�f�[�^�̍Đ�
	if (isPause == false) {
		if (pSourceVoice == nullptr) {
			assert(0);
			return;
		}
		result = pSourceVoice->FlushSourceBuffers();
		result = pSourceVoice->SubmitSourceBuffer(&buf);
	}

	result = pSourceVoice->Start();

	isPause = false;
}

void SourceVoice::StopWave()
{
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->Stop();
	if (GetIsLoop()) {
		result - pSourceVoice->ExitLoop();
	}
}

void SourceVoice::PauseWave()
{
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->Stop(XAUDIO2_PLAY_TAILS);
	isPause = true;
}

void SourceVoice::Update3DAudio()
{
	DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
		| X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
		| X3DAUDIO_CALCULATE_REVERB;

	X3DAudioCalculate(*Sound::GetPX3DInstance(), Sound::GetPListener(), &emitter,
		dwCalcFlags,
		&dspSettings);

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	pSourceVoice->SetFrequencyRatio(dspSettings.DopplerFactor * pitch);

	if (submixVoiceList.size() <= 0) {
		pSourceVoice->SetOutputMatrix(Sound::GetPMasterVoice(), voiceDetails.InputChannels,
			Sound::GetDeviceDetails().InputChannels, dspSettings.pMatrixCoefficients);
	}

	for (int i = 0; i < submixVoiceList.size(); i++) {
		pSourceVoice->SetOutputMatrix(submixVoiceList[i]->GetPXAudio2SubmixVoice(), voiceDetails.InputChannels,
			Sound::GetDeviceDetails().InputChannels, dspSettings.pMatrixCoefficients);
		pSourceVoice->SetOutputMatrix(submixVoiceList[i]->GetPXAudio2SubmixVoice(),
			1, 1, &dspSettings.ReverbLevel);
	}
	
	XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * dspSettings.LPFDirectCoefficient), 1.0f };
	pSourceVoice->SetOutputFilterParameters(Sound::GetPMasterVoice(), &FilterParameters);
}

void SourceVoice::AddOutputSubmixVoice(SubmixVoice* pSubmixVoice)
{
	//���łɑ��݂���Ƃ��̓��^�[��
	if (std::find(submixVoiceList.begin(), submixVoiceList.end(), pSubmixVoice) != submixVoiceList.end()) {
		return;
	}

	submixVoiceList.push_back(pSubmixVoice);

	//�o�͐�؂�ւ�
	std::vector<XAUDIO2_SEND_DESCRIPTOR> send;

	for (auto&& v : submixVoiceList) {
		send.push_back({ XAUDIO2_SEND_USEFILTER, v->GetPXAudio2SubmixVoice() });
	}

	//������
	XAUDIO2_VOICE_SENDS sendlist = { send.size(), send.data() };

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetOutputVoices(&sendlist);

	send.clear();
}

void SourceVoice::ClearOutputSubmixVoice()
{
	submixVoiceList.clear();
	//�o�͐�؂�ւ�
	XAUDIO2_SEND_DESCRIPTOR send = { XAUDIO2_SEND_USEFILTER, Sound::GetPMasterVoice() };

	XAUDIO2_VOICE_SENDS sendlist = { 1, &send };
	//�}�X�^�[�{�C�X�ɏo��
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetOutputVoices(&sendlist);
}

void SourceVoice::AddXAPOEffect(XAPOEffect* pEffect)
{
	std::vector<XAUDIO2_EFFECT_DESCRIPTOR> desc;

	//�G�t�F�N�g���X�g�ɓo�^
	effectIndex.emplace(pEffect, effectIndex.size());

	for (auto&& v : effectIndex) {
		desc.push_back(v.first->GetEffectDescriptor());
	}

	//EffectChain�̍쐬
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = desc.size(); //������
	chain.pEffectDescriptors = desc.data();

	// �{�C�X�� EFFECT_CHAIN ��}��
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetEffectChain(&chain);

	desc.clear();
}

void SourceVoice::ClearXAPOEffectList()
{
	effectIndex.clear();

	//EffectChain�̍쐬
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 0;
	chain.pEffectDescriptors = nullptr;

	// �{�C�X�� EFFECT_CHAIN ��}��
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetEffectChain(&chain);
}

void SourceVoice::EnableXAPOEffect(XAPOEffect* pEffect)
{
	//�o�^����Ă��Ȃ������烊�^�[��
	if (effectIndex.find(pEffect) == effectIndex.end()) {
		return;
	}

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->EnableEffect(effectIndex[pEffect]);
}

void SourceVoice::DisableXAPOEffect(XAPOEffect* pEffect)
{
	//�o�^����Ă��Ȃ������烊�^�[��
	if (effectIndex.find(pEffect) == effectIndex.end()) {
		return;
	}

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->DisableEffect(effectIndex[pEffect]);
}

void SourceVoice::SetVolume(const float volume)
{
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetVolume(volume);
}

void SourceVoice::SetPitch(const float pitch)
{
	//�N���b�s���O
	if (pitch > maxPitchRate) {
		this->pitch = maxPitchRate;
	}
	else if (pitch < XAUDIO2_MIN_FREQ_RATIO){
		this->pitch = XAUDIO2_MIN_FREQ_RATIO;
	}
	else {
		this->pitch = pitch;
	}

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetFrequencyRatio(this->pitch);
}

void SourceVoice::SetPan(const float pan, SubmixVoice* pDstVoice)
{
	HRESULT result = S_FALSE;
	float lpan = pan;

	//�N���b�s���O
	if (lpan < -1.0f) {
		lpan = -1.0f;
	}
	else if (lpan > 1.0f) {
		lpan = 1.0f;
	}

	//�p�����O�p�֐��Ōv�Z
	float dig = (lpan + 1.0f) * 45.0f;

	SetPanFromAngle(dig, pDstVoice);
}
void SourceVoice::SetPanFromAngle(const float dig, SubmixVoice* pDstVoice)
{
	HRESULT result = S_FALSE;

	float outputMatrix[8];
	for (int i = 0; i < 8; i++) {
		outputMatrix[i] = 0;
	}

	//�p�����O�p�֐��Ōv�Z
	float rad = (dig / 2) * 3.14159265f / 180;

	float right = cos(rad);
	float left = sin(rad);

	//�X�s�[�J�[�z�u�ɍ��킹�ēK�p
	switch (Sound::GetChannelMask())
	{
	case KSAUDIO_SPEAKER_MONO:
		outputMatrix[0] = 1.0;
		break;
	case KSAUDIO_SPEAKER_STEREO:
	case KSAUDIO_SPEAKER_2POINT1:
	case KSAUDIO_SPEAKER_SURROUND:
		outputMatrix[0] = left;
		outputMatrix[3] = right;
		break;
	case KSAUDIO_SPEAKER_QUAD:
		outputMatrix[0] = outputMatrix[2] = left;
		outputMatrix[1] = outputMatrix[3] = right;
		break;
	case KSAUDIO_SPEAKER_5POINT1:
	case KSAUDIO_SPEAKER_7POINT1:
		outputMatrix[0] = outputMatrix[4] = left;
		outputMatrix[1] = outputMatrix[5] = right;
		break;
	}

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}

	//�T�u�~�b�N�X�{�C�X�̎w�肪�Ȃ��ꍇ��NULL
	if (pDstVoice == nullptr) {
		result = pSourceVoice->SetOutputMatrix(NULL, voiceDetails.InputChannels, Sound::GetDeviceDetails().InputChannels, outputMatrix);
	}
	else {
		result = pSourceVoice->SetOutputMatrix(pDstVoice->GetPXAudio2SubmixVoice(), voiceDetails.InputChannels, Sound::GetDeviceDetails().InputChannels, outputMatrix);
	}
}

void SourceVoice::SetReverbParamsFromPreset(XAPOEffect* pEffect, const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& preset)
{
	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	ReverbConvertI3DL2ToNative(&preset, &reverbParameters);

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetEffectParameters(effectIndex[pEffect], &reverbParameters, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
}

void SourceVoice::SetReverbParams(XAPOEffect* pEffect, XAUDIO2FX_REVERB_PARAMETERS* p)
{
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}

	if (p == nullptr) {
		XAUDIO2FX_REVERB_PARAMETERS params;

		params.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
		params.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
		params.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
		params.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		params.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
		params.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		params.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
		params.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
		params.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
		params.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
		params.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
		params.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
		params.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
		params.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
		params.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
		params.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
		params.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
		params.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
		params.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
		params.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
		params.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
		params.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;

		HRESULT result = pSourceVoice->SetEffectParameters(effectIndex[pEffect], &params, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
	}
	else {
		HRESULT result = pSourceVoice->SetEffectParameters(effectIndex[pEffect], p, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
	}
}

void SourceVoice::SetLoopConfig(bool isLoop, const int loopCount, const float loopStartSec, const float loopEndSec)
{
	if (isLoop) {
		buf.LoopCount = (UINT32)loopCount;
		buf.LoopBegin = (UINT32)Sound::ConvertMillisecondToSample(loopStartSec, pWaveData->GetSamplingRate());
		buf.LoopLength = (UINT32)Sound::ConvertMillisecondToSample(loopEndSec, pWaveData->GetSamplingRate()) - buf.LoopBegin;
	}
	else {
		buf.LoopCount = 0;
	}
}

void SourceVoice::SetFilterEffect(const XAUDIO2_FILTER_TYPE& type, const float frequency, const bool isSetAsHz, const float oneOverQ)
{
	//�G�t�F�N�g���g�p���Ȃ��ݒ�̎��̓��^�[��
	if (isUseEffect != true) {
		return;
	}

	float lfrequency = frequency;
	//�t�B���^�G�t�F�N�g���Z�b�g
	XAUDIO2_FILTER_PARAMETERS filterParams;
	filterParams.Type = type;
	if (isSetAsHz) {
		if (type != XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter &&
			type != XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter) {
			//�ȈՎ�
			filterParams.Frequency = lfrequency / pWaveData->GetSamplingRate() * 6.0f;
		}
		else {
			//�ő�l�̓T���v�����O���[�g��1/2
			if (lfrequency > pWaveData->GetSamplingRate() * 0.5f) {
				lfrequency = pWaveData->GetSamplingRate() * 0.5f;
			}

			filterParams.Frequency = XAudio2CutoffFrequencyToOnePoleCoefficient(lfrequency, (UINT32)pWaveData->GetSamplingRate());
		}
	}
	else {
		//0.0f�`1.0f�̊ԂŌ��ߑł�
		filterParams.Frequency = lfrequency;
	}
	filterParams.OneOverQ = oneOverQ;

	//�N���b�s���O
	if (filterParams.Frequency > XAUDIO2_MAX_FILTER_FREQUENCY) {
		filterParams.Frequency = XAUDIO2_MAX_FILTER_FREQUENCY;
	}
	else if (filterParams.Frequency < 0) {
		filterParams.Frequency = 0;
	}
	if (filterParams.OneOverQ > XAUDIO2_MAX_FILTER_ONEOVERQ) {
		filterParams.OneOverQ = XAUDIO2_MAX_FILTER_ONEOVERQ;
	}
	//oneOverQ��0�ȉ���NG
	else if (filterParams.OneOverQ < 0.1f) {
		filterParams.OneOverQ = 0.1f;
	}

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}

	HRESULT result = pSourceVoice->SetFilterParameters(&filterParams);
}

void SourceVoice::Set3DEmitterPos(const float x, const float y, const float z)
{
	emitter.Position.x = x;
	emitter.Position.y = y;
	emitter.Position.z = z;

	emitter.Velocity.x = emitter.Position.x - emitterPrevPos.x;
	emitter.Velocity.y = emitter.Position.y - emitterPrevPos.y;
	emitter.Velocity.z = emitter.Position.z - emitterPrevPos.z;

	emitterPrevPos.x = emitter.Position.x;
	emitterPrevPos.y = emitter.Position.y;
	emitterPrevPos.z = emitter.Position.z;
}

void SourceVoice::Set3DEmitterFrontVec(const float x, const float y, const float z)
{
	emitter.OrientFront.x = x;
	emitter.OrientFront.y = y;
	emitter.OrientFront.z = z;
}

void SourceVoice::Set3DEmitterUpVec(const float x, const float y, const float z)
{
	emitter.OrientTop.x = x;
	emitter.OrientTop.y = y;
	emitter.OrientTop.z = z;
}

void SourceVoice::Set3DEmitterDistanceScaler(const float scalar)
{
	emitter.CurveDistanceScaler = scalar;
}

void SourceVoice::Set3DEmitterDoppelScaler(const float scaler)
{
	emitter.DopplerScaler = scaler;
}

void SourceVoice::Set3DEmitterInnerRadius(const float r)
{
	emitter.InnerRadius = r;
}

void SourceVoice::Set3DEmitterInnerAngle(const float digrees)
{
	emitter.InnerRadiusAngle = digrees * X3DAUDIO_PI / 180;
}

void SourceVoice::DisableFilterEffect()
{
	//�G�t�F�N�g���g�p���Ȃ��ݒ�̎��̓��^�[��
	if (isUseEffect != true) {
		return;
	}

	//�t�B���^�[�G�t�F�N�g�𖳌��ɂ���ݒ�
	XAUDIO2_FILTER_PARAMETERS FilterParams;
	FilterParams.Type = XAUDIO2_FILTER_TYPE::LowPassFilter;
	FilterParams.Frequency = 1.0f;
	FilterParams.OneOverQ = 1.0f;

	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetFilterParameters(&FilterParams);
}

float SourceVoice::GetPlayingPosition()
{
	if (pSourceVoice == nullptr) {
		assert(0);
	}

	//��܂��Ȃ���
	XAUDIO2_VOICE_STATE xvs;
	pSourceVoice->GetState(&xvs);
	return (float)xvs.SamplesPlayed / (pWaveData->GetSamplingRate() / 1000.0f);
}

float SourceVoice::GetVolume()
{
	if (pSourceVoice == nullptr) {
		assert(0);
	}

	float vol;
	pSourceVoice->GetVolume(&vol);
	return vol;
}

bool SourceVoice::GetIsPlay()
{
	if (pSourceVoice == nullptr) {
		assert(0);
	}

	XAUDIO2_VOICE_STATE xvs;
	pSourceVoice->GetState(&xvs);
	return xvs.BuffersQueued != 0;
}

bool SourceVoice::GetIsLoop()
{
	return buf.LoopCount != 0;
}