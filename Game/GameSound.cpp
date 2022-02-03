#include "GameSound.h"

static const std::wstring SE_DIR = L"Resources/Sound/SE/";
static const std::wstring BGM_DIR = L"Resources/Sound/BGM/";

std::unordered_map<std::wstring, GameSound::SoundData> GameSound::sounds;
const float GameSound::MASTER_DISTANCE = 250;

void GameSound::StaticInitialize()
{
	//BGM�ǂݍ���
	AddSound(BGM_DIR + L"Title.wav", false, true, 0, 133575);
	AddSound(BGM_DIR + L"GamePlay.wav", false, true, 0, 75139);

	//SE�ǂݍ���
	AddSound(SE_DIR + L"BreakBlock.wav");
	AddSound(SE_DIR + L"BreakFloor.wav");
	AddSound(SE_DIR + L"Reflect.wav");
	AddSound(SE_DIR + L"Shoot.wav");
	AddSound(SE_DIR + L"Shooting.wav", true, true, 500, 1500);
	AddSound(SE_DIR + L"Crack.wav");
	AddSound(SE_DIR + L"ChangeColor.wav");
	AddSound(SE_DIR + L"StageDecide.wav", false);
	AddSound(SE_DIR + L"GameStart.wav", false);
	AddSound(SE_DIR + L"PosDecide.wav", false);
	AddSound(SE_DIR + L"RollNumber.wav", false, true, 0, 127);
	AddSound(SE_DIR + L"Ascension.wav", false);
	AddSound(SE_DIR + L"Clear.wav", false);
	AddSound(SE_DIR + L"UI_Select.wav", false);
	AddSound(SE_DIR + L"UI_Click.wav", false);

	//���V���ʉ���������ɉ����܂ŋ�������
	sounds[L"Ascension"].sourceVoice.Set3DEmitterDistanceScaler(MASTER_DISTANCE * 2);
}

void GameSound::Update()
{
	for (auto itr = sounds.begin(); itr != sounds.end(); itr++) {
		//3D�I�[�f�B�I�p�A�b�v�f�[�g
		if (itr->second.isUse3D) {
			itr->second.sourceVoice.Update3DAudio();
		}
		//�^�C�}�[�X�V
		itr->second.stopTimer.Update();

		//Stop���ߏo����Ă�����
		if (itr->second.stopTimer.GetNowTime() > itr->second.stopTimer.GetStartTime()) {
			if (itr->second.stopTimer.GetNowTime() < itr->second.stopTimer.GetEndTime()) {
				float volume = 1.0f - (float)itr->second.stopTimer.GetNowTime() / itr->second.stopTimer.GetEndTime();
				itr->second.sourceVoice.SetVolume(volume);
			}
			else {
				//������~
				itr->second.sourceVoice.StopWave();
				//�{�����ɖ߂�
				itr->second.sourceVoice.SetVolume(1.0);
				//�^�C�}�[���Z�b�g
				itr->second.stopTimer.Reset();
			}
		}
	}

}

void GameSound::AddSound(const std::wstring& path, bool isUse3D, bool isLoop, float loopStartPos, float loopEndPos)
{
	//�������擾
	std::wstring wavName = path;
	//�u/�v�Ō���
	int findSlash = path.rfind(L"/");
	if (findSlash != std::wstring::npos) {
		wavName = path.substr(findSlash + 1);
	}
	else {
		//�u\�v�Ō���
		findSlash = path.rfind(L"\\");
		if (findSlash != std::wstring::npos) {
			wavName = path.substr(findSlash + 1);
		}
		else {
			//�t�@�C����.exe�Ɠ����K�w�ɂ���
			wavName = path;
		}
	}

	//.wav�𔲂����
	wavName = wavName.substr(0, wavName.size() - 4);

	//�T�E���h�ǉ�
	sounds[wavName].waveData.LoadWave(path.c_str());
	sounds[wavName].sourceVoice.CreateSourceVoice(&sounds[wavName].waveData);

	//�e��ݒ�
	sounds[wavName].sourceVoice.Set3DEmitterDistanceScaler(MASTER_DISTANCE);
	if (isLoop) {
		sounds[wavName].sourceVoice.SetLoopConfig(true, 255, loopStartPos, loopEndPos);
	}

	//�^�C�}�[������
	sounds[wavName].stopTimer.SetTimer(0, 100);

	//3D�I�[�f�B�I���ʂ��g�p���邩
	sounds[wavName].isUse3D = isUse3D;
}

void GameSound::Play(const std::wstring& name)
{
	//3D�����̃G�~�b�^�ʒu�Z�b�g�i���X�i�[�Ɠ��ʒu�j
	sounds[name].sourceVoice.Set3DEmitterPos(Sound::GetPListener()->Position.x,Sound::GetPListener()->Position.y, Sound::GetPListener()->Position.z);
	//�Đ�
	sounds[name].sourceVoice.PlayWave();
}

void GameSound::Play(const std::wstring& name, const Vector3& emitterPos)
{
	//3D�����̃G�~�b�^�ʒu�Z�b�g
	sounds[name].sourceVoice.Set3DEmitterPos(emitterPos.x, emitterPos.y, emitterPos.z);
	//�Đ�
	sounds[name].sourceVoice.PlayWave();
}

void GameSound::Stop(const std::wstring& name, int fadeOutMs)
{
	//�}�ɉ����Ȃ�����
	if (sounds[name].stopTimer.GetIsStart() == false) {
		sounds[name].stopTimer.SetTimer(0, fadeOutMs);
		sounds[name].stopTimer.Start();
	}
}

void GameSound::SetVolume(const std::wstring& name, float volume)
{
	sounds[name].sourceVoice.SetVolume(volume);
}

void GameSound::SetPosition(const std::wstring& name, const Vector3& pos)
{
	sounds[name].sourceVoice.Set3DEmitterPos(pos.x, pos.y, pos.z);
}

void GameSound::SetDistance(float distance)
{
	for (auto itr = sounds.begin(); itr != sounds.end(); itr++) {
		itr->second.sourceVoice.Set3DEmitterDistanceScaler(distance);
	}
}

SourceVoice& GameSound::GetLoadedSound(const std::wstring& name)
{
	return sounds[name].sourceVoice;
}
