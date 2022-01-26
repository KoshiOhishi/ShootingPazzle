#include "Sound.h"
#include <fstream>
#include <cassert>
#include <mmsystem.h>
#include "Quaternion.h"
#include "Vector3.h"

#pragma comment ( lib, "winmm.lib" )

//静的メンバ変数の実体
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

	//COMの初期化
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		assert(0);
		//解放
		CoUninitialize();
		return;
	}

	//XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result))
	{
		assert(0);
		//解放
		CoUninitialize();
		return;
	}

	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&pMasterVoice);
	if (FAILED(result))
	{
		assert(0);
		StaticFinalize();
		return;
	}

	//チャンネルマスク取得
	result = pMasterVoice->GetChannelMask(&dwChannelMask);
	if (FAILED(result))
	{
		assert(0);
		StaticFinalize();
		return;
	}

	//デバイスデータ取得
	pMasterVoice->GetVoiceDetails(&deviceDetails);

	if (use3DAudio) {
		//X3DAudio初期化
		X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance);

		//リスナー初期化
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
	//ソースボイス後始末
	for (int i = 0; i < sourceVoiceManager.size(); i++) {
		sourceVoiceManager[i]->DestroySourceVoice();
	}
	//サブミックスボイス後始末
	for (int i = 0; i < submixVoiceManager.size(); i++) {
		submixVoiceManager[i]->DestroySubmixVoice();
	}
	//XAPOEffect後始末
	for (int i = 0; i < xapoEffectManager.size(); i++) {
		xapoEffectManager[i]->DestroyXAPOEffect();
	}
	//WaveData後始末
	for (int i = 0; i < waveDataManager.size(); i++) {
		waveDataManager[i]->DeleteWave();
	}

	//マスターボイス解放
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

	//正規化
	front = front.Normalize();

	//正規化
	up = up.Normalize();

	//回転行列で回転
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
	//同一要素が存在していなかったら追加
	if (std::find(sourceVoiceManager.begin(), sourceVoiceManager.end(), pSourceVoice) == sourceVoiceManager.end()) {
		sourceVoiceManager.emplace_back(pSourceVoice);
	}
}

void Sound::AddSubmixVoice(SubmixVoice* pSubmixVoice)
{
	//同一要素が存在していなかったら追加
	if (std::find(submixVoiceManager.begin(), submixVoiceManager.end(), pSubmixVoice) == submixVoiceManager.end()) {
		submixVoiceManager.emplace_back(pSubmixVoice);
	}
}

void Sound::AddWaveData(WaveData* pWaveData)
{
	//同一要素が存在していなかったら追加
	if (std::find(waveDataManager.begin(), waveDataManager.end(), pWaveData) == waveDataManager.end()) {
		waveDataManager.emplace_back(pWaveData);
	}
}

void Sound::AddXAPOEffect(XAPOEffect* pXAPOEffect)
{
	//同一要素が存在していなかったら追加
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
	//XAPO生成
	IUnknown* pXAPO;
	XAudio2CreateReverb(&pXAPO);

	//デスクリプタの作成
	effectDesc.InitialState = initialState;
	effectDesc.OutputChannels = 2; //とりあえず2ch決め打ち
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
	//引数の情報をもとにサブミックスボイス生成
	HRESULT result = Sound::GetXAudio2Instance()->CreateSubmixVoice(&pSubmixVoice, (UINT32)channelCount, (UINT32)samplingRate);
	
	Sound::AddSubmixVoice(this);
}

void SubmixVoice::DestroySubmixVoice()
{
	//破棄
	if (pSubmixVoice) {
		ClearXAPOEffect();

		pSubmixVoice->DestroyVoice();
		pSubmixVoice = nullptr;
	}
}

void SubmixVoice::AddXAPOEffect(XAPOEffect* pEffect)
{
	std::vector<XAUDIO2_EFFECT_DESCRIPTOR> desc;

	//エフェクトリストに登録
	effectIndexSubmix.emplace(pEffect, effectIndexSubmix.size());

	for (auto&& v : effectIndexSubmix) {
		desc.push_back(v.first->GetEffectDescriptor());
	}

	//EffectChainの作成
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = desc.size(); //複数版
	chain.pEffectDescriptors = desc.data();

	// ボイスに EFFECT_CHAIN を挿す
	HRESULT result = pSubmixVoice->SetEffectChain(&chain);

	desc.clear();
}

void SubmixVoice::ClearXAPOEffect()
{
	effectIndexSubmix.clear();

	//EffectChainの作成
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 0;
	chain.pEffectDescriptors = nullptr;

	// ボイスに EFFECT_CHAIN を挿す
	HRESULT result = pSubmixVoice->SetEffectChain(&chain);

}

void SubmixVoice::EnableXAPOEffect(XAPOEffect* pEffect)
{
	//登録されていなかったらリターン
	if (effectIndexSubmix.find(pEffect) == effectIndexSubmix.end()) {
		return;
	}

	HRESULT result = pSubmixVoice->EnableEffect(effectIndexSubmix[pEffect]);
}

void SubmixVoice::DisableXAPOEffect(XAPOEffect* pEffect)
{
	//登録されていなかったらリターン
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

	//wavファイルを開く
	HMMIO mmioHandle = mmioOpen((LPWSTR)filename, NULL, MMIO_READ);
	if (mmioHandle == NULL) {
		assert(0);
	}

	MMCKINFO riffinfo;
	riffinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	//RIFFチャンクに進入
	mmResult = mmioDescend(mmioHandle, &riffinfo, NULL, MMIO_FINDRIFF);
	if (mmResult != NOERROR) {
		assert(0);
		//クローズ
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//fmtチャンク
	MMCKINFO ckinfo;
	ckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmResult = mmioDescend(mmioHandle, &ckinfo, &riffinfo, MMIO_FINDCHUNK);
	if (mmResult != NOERROR) {
		assert(0);
		//クローズ
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//PCMWAVEFORMAT取得
	PCMWAVEFORMAT pwf;
	LONG size = mmioRead(mmioHandle, (HPSTR)&pwf, sizeof(pwf));
	//読み込みサイズが正しいかチェック
	if (size != sizeof(pwf)) {
		assert(0);
		//クローズ
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//余分なBYTEがあるか
	if (pwf.wf.wFormatTag == WAVE_FORMAT_PCM) {
		wfex = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX)];
		if (wfex == NULL) {
			assert(0);
			//クローズ
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}

		//WAVEFORMATEXにコピー
		memcpy(wfex, &pwf, sizeof(pwf));
		wfex->cbSize = 0;
	}
	else {
		//余分なBYTE読み込み
		WORD cbExtraBytes = 0L;
		size = mmioRead(mmioHandle, (CHAR*)&cbExtraBytes, sizeof(WORD));
		if (size != sizeof(WORD)) {
			assert(0);
			//クローズ
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}

		wfex = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX) + cbExtraBytes];
		if (wfex == NULL) {
			assert(0);
			//クローズ
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}

		//WAVEFORMATEXにコピー
		memcpy(wfex, &pwf, sizeof(pwf));
		wfex->cbSize = cbExtraBytes;

		//余分なBYTE読み込み
		size = mmioRead(mmioHandle, (CHAR*)(((BYTE*)&(wfex->cbSize)) + sizeof(WORD)), cbExtraBytes);
		if (size != cbExtraBytes) {
			assert(0);
			//クローズ
			mmioClose(mmioHandle, MMIO_FHOPEN);
			return;
		}
	}

	//読み込めたらfmtチャンクを抜ける
	mmResult = mmioAscend(mmioHandle, &ckinfo, 0);
	if (mmResult != MMSYSERR_NOERROR) {
		assert(0);
		//クローズ
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//dataチャンク
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmResult = mmioDescend(mmioHandle, &ckinfo, &riffinfo, MMIO_FINDCHUNK);
	if (mmResult != NOERROR) {
		assert(0);
		//クローズ
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//音データの読み込み
	soundBuffer = new char[ckinfo.cksize];
	size = mmioRead(mmioHandle, (HPSTR)soundBuffer, ckinfo.cksize);
	if (size != ckinfo.cksize) {
		delete[] soundBuffer;
		assert(0);
		//クローズ
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return;
	}

	//クローズ
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

	//クリッピング
	if (maxPitchRate > XAUDIO2_MAX_FREQ_RATIO) {
		this->maxPitchRate = XAUDIO2_MAX_FREQ_RATIO;
	}
	else if (maxPitchRate < 1.0f) {
		this->maxPitchRate = 1.0f;
	}
	else {
		this->maxPitchRate = maxPitchRate;
	}

	//波形フォーマットを元にSourceVoiceの生成
	if (isUseEffect) {
		//エフェクト使用設定
		result = Sound::GetXAudio2Instance()->CreateSourceVoice(&pSourceVoice, this->pWaveData->GetPWaveFormatEx(), XAUDIO2_VOICE_USEFILTER, this->maxPitchRate, &callback);
	}
	else {
		//エフェクトを使わない設定
		result = Sound::GetXAudio2Instance()->CreateSourceVoice(&pSourceVoice, this->pWaveData->GetPWaveFormatEx(), 0, this->maxPitchRate, &callback);
	}
	this->isUseEffect = isUseEffect;

	//再生する波形データの設定
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

	//波形データの再生
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
	//すでに存在するときはリターン
	if (std::find(submixVoiceList.begin(), submixVoiceList.end(), pSubmixVoice) != submixVoiceList.end()) {
		return;
	}

	submixVoiceList.push_back(pSubmixVoice);

	//出力先切り替え
	std::vector<XAUDIO2_SEND_DESCRIPTOR> send;

	for (auto&& v : submixVoiceList) {
		send.push_back({ XAUDIO2_SEND_USEFILTER, v->GetPXAudio2SubmixVoice() });
	}

	//複数版
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
	//出力先切り替え
	XAUDIO2_SEND_DESCRIPTOR send = { XAUDIO2_SEND_USEFILTER, Sound::GetPMasterVoice() };

	XAUDIO2_VOICE_SENDS sendlist = { 1, &send };
	//マスターボイスに出力
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetOutputVoices(&sendlist);
}

void SourceVoice::AddXAPOEffect(XAPOEffect* pEffect)
{
	std::vector<XAUDIO2_EFFECT_DESCRIPTOR> desc;

	//エフェクトリストに登録
	effectIndex.emplace(pEffect, effectIndex.size());

	for (auto&& v : effectIndex) {
		desc.push_back(v.first->GetEffectDescriptor());
	}

	//EffectChainの作成
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = desc.size(); //複数版
	chain.pEffectDescriptors = desc.data();

	// ボイスに EFFECT_CHAIN を挿す
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

	//EffectChainの作成
	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 0;
	chain.pEffectDescriptors = nullptr;

	// ボイスに EFFECT_CHAIN を挿す
	if (pSourceVoice == nullptr) {
		assert(0);
		return;
	}
	HRESULT result = pSourceVoice->SetEffectChain(&chain);
}

void SourceVoice::EnableXAPOEffect(XAPOEffect* pEffect)
{
	//登録されていなかったらリターン
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
	//登録されていなかったらリターン
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
	//クリッピング
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

	//クリッピング
	if (lpan < -1.0f) {
		lpan = -1.0f;
	}
	else if (lpan > 1.0f) {
		lpan = 1.0f;
	}

	//パンを三角関数で計算
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

	//パンを三角関数で計算
	float rad = (dig / 2) * 3.14159265f / 180;

	float right = cos(rad);
	float left = sin(rad);

	//スピーカー配置に合わせて適用
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

	//サブミックスボイスの指定がない場合はNULL
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
	//エフェクトを使用しない設定の時はリターン
	if (isUseEffect != true) {
		return;
	}

	float lfrequency = frequency;
	//フィルタエフェクトをセット
	XAUDIO2_FILTER_PARAMETERS filterParams;
	filterParams.Type = type;
	if (isSetAsHz) {
		if (type != XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter &&
			type != XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter) {
			//簡易式
			filterParams.Frequency = lfrequency / pWaveData->GetSamplingRate() * 6.0f;
		}
		else {
			//最大値はサンプリングレートの1/2
			if (lfrequency > pWaveData->GetSamplingRate() * 0.5f) {
				lfrequency = pWaveData->GetSamplingRate() * 0.5f;
			}

			filterParams.Frequency = XAudio2CutoffFrequencyToOnePoleCoefficient(lfrequency, (UINT32)pWaveData->GetSamplingRate());
		}
	}
	else {
		//0.0f～1.0fの間で決め打ち
		filterParams.Frequency = lfrequency;
	}
	filterParams.OneOverQ = oneOverQ;

	//クリッピング
	if (filterParams.Frequency > XAUDIO2_MAX_FILTER_FREQUENCY) {
		filterParams.Frequency = XAUDIO2_MAX_FILTER_FREQUENCY;
	}
	else if (filterParams.Frequency < 0) {
		filterParams.Frequency = 0;
	}
	if (filterParams.OneOverQ > XAUDIO2_MAX_FILTER_ONEOVERQ) {
		filterParams.OneOverQ = XAUDIO2_MAX_FILTER_ONEOVERQ;
	}
	//oneOverQは0以下でNG
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
	//エフェクトを使用しない設定の時はリターン
	if (isUseEffect != true) {
		return;
	}

	//フィルターエフェクトを無効にする設定
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

	//大まかなもの
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