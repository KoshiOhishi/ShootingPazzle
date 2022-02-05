#include "GameSound.h"

static const std::wstring SE_DIR = L"Resources/Sound/SE/";
static const std::wstring BGM_DIR = L"Resources/Sound/BGM/";

std::vector<std::unique_ptr<GameSound::SoundData>> GameSound::sounds;
std::unordered_map<std::wstring, int> GameSound::indexes;
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
	sounds[indexes[L"Ascension"]]->sourceVoice->Set3DEmitterDistanceScaler(MASTER_DISTANCE * 2);
}

void GameSound::StaticFinalize()
{
	indexes.clear();
	for (auto& v : sounds) {
		v.release();
	}
	sounds.clear();
}

void GameSound::Update()
{
	for (auto& v : sounds) {
		//3D�I�[�f�B�I�p�A�b�v�f�[�g
		if (v->isUse3D) {
			v->sourceVoice->Update3DAudio();
		}
		//�^�C�}�[�X�V
		v->stopTimer.Update();

		//Stop���ߏo����Ă�����
		if (v->stopTimer.GetNowTime() > v->stopTimer.GetStartTime()) {
			if (v->stopTimer.GetNowTime() < v->stopTimer.GetEndTime()) {
				float volume = 1.0f - (float)v->stopTimer.GetNowTime() / v->stopTimer.GetEndTime();
				v->sourceVoice->SetVolume(volume);
			}
			else {
				//������~
				v->sourceVoice->StopWave();
				//�{�����ɖ߂�
				v->sourceVoice->SetVolume(1.0);
				//�^�C�}�[���Z�b�g
				v->stopTimer.Reset();
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

	//�C���f�b�N�X���ǉ�
	indexes.emplace(wavName, sounds.size());

	//�T�E���h�ǉ�
	std::unique_ptr<SoundData> pData = std::make_unique<SoundData>();
	SoundData& data = *pData;

	data.waveData = std::make_unique<WaveData>();
	data.waveData->LoadWave(path.c_str());

	data.sourceVoice = std::make_unique<SourceVoice>();
	data.sourceVoice->CreateSourceVoice(data.waveData.get());

	//�e��ݒ�
	data.sourceVoice->Set3DEmitterDistanceScaler(MASTER_DISTANCE);
	if (isLoop) {
		data.sourceVoice->SetLoopConfig(true, 255, loopStartPos, loopEndPos);
	}

	//�^�C�}�[������
	data.stopTimer.SetTimer(0, 100);

	//3D�I�[�f�B�I���ʂ��g�p���邩
	data.isUse3D = isUse3D;

	sounds.emplace_back(std::move(pData));
}

void GameSound::Play(const std::wstring& name)
{
	int index = indexes[name];
	//3D�����̃G�~�b�^�ʒu�Z�b�g�i���X�i�[�Ɠ��ʒu�j
	sounds[index]->sourceVoice->Set3DEmitterPos(Sound::GetPListener()->Position.x,Sound::GetPListener()->Position.y, Sound::GetPListener()->Position.z);
	//�Đ�
	sounds[index]->sourceVoice->PlayWave();
}

void GameSound::Play(const std::wstring& name, const Vector3& emitterPos)
{
	int index = indexes[name];
	//3D�����̃G�~�b�^�ʒu�Z�b�g
	sounds[index]->sourceVoice->Set3DEmitterPos(emitterPos.x, emitterPos.y, emitterPos.z);
	//�Đ�
	sounds[index]->sourceVoice->PlayWave();
}

void GameSound::Stop(const std::wstring& name, int fadeOutMs)
{
	int index = indexes[name];
	//�}�ɉ����Ȃ�����
	if (sounds[index]->stopTimer.GetIsStart() == false) {
		sounds[index]->stopTimer.SetTimer(0, fadeOutMs);
		sounds[index]->stopTimer.Start();
	}
}

bool GameSound::IsPlaying(const std::wstring& name)
{
	int index = indexes[name];
	return sounds[index]->sourceVoice->GetIsPlay();
}
void GameSound::SetVolume(const std::wstring& name, float volume)
{
	int index = indexes[name];
	sounds[index]->sourceVoice->SetVolume(volume);
}

void GameSound::SetPosition(const std::wstring& name, const Vector3& pos)
{
	int index = indexes[name];
	sounds[index]->sourceVoice->Set3DEmitterPos(pos.x, pos.y, pos.z);
}

void GameSound::SetDistance(float distance)
{
	for (auto& v : sounds) {
		v->sourceVoice->Set3DEmitterDistanceScaler(distance);
	}
}

SourceVoice& GameSound::GetLoadedSound(const std::wstring& name)
{
	int index = indexes[name];
	return *sounds[index]->sourceVoice.get();
}
