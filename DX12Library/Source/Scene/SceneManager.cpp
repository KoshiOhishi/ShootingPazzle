#include <assert.h>
#include "Input.h"
#include "SceneManager.h"
#include "ImguiHelper.h"
#include "PostEffect.h"
#include "DebugText.h"
#include "DrawManager.h"
#include "Timer.h"

using namespace DX12Library;

//�ÓI�����o�ϐ��̎���
std::string SceneManager::nowScene;
std::string SceneManager::nextScene = "";
std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::scenes;

void SceneManager::AddScene(Scene* scene, const string& sceneName)
{
	scene->SetSceneName(sceneName);
	scenes.emplace(sceneName, scene);
}

void SceneManager::ChangeScene(const string& sceneName)
{
	//�V�[�������������玟�̃t���[������̃V�[���Z�b�g
	if (scenes.find(sceneName) != scenes.end()){
		nextScene = sceneName;
	}
}

void SceneManager::DeleteScene()
{
	scenes.clear();
}

void SceneManager::Initialize()
{
	scenes[nowScene]->Initialize();
}

void SceneManager::Update()
{
	//�t���[���̓��ŃV�[���؂�ւ�
	if (nowScene != nextScene) {
		nowScene = nextScene;
		Initialize();
	}

	//�^�C�}�[�A�b�v�f�[�g
	Timer::UpdateAll();

	scenes[nowScene]->Update();
}

void SceneManager::Draw()
{
	//���X�g�ɒǉ�
	scenes[nowScene]->Draw();

	//�f�o�b�O�e�L�X�g�̃X�v���C�g�O�i�`��o�^
	DebugText::DrawAll();

	//�܂��[�x�l�e�N�X�`����������
	DrawManager::DrawAllShadow();

	//���Ƀ|�X�g�G�t�F�N�g�p�e�N�X�`���ɕ`��
	PostEffect::PreDrawScene();
	//�w�i�`��
	DrawManager::DrawAll(true);

	//�[�x�o�b�t�@�N���A
	DX12Util::ClearDepthBuffer(false);

	//�ʏ�`��
	DrawManager::DrawAll();

	//Imgui�`��
	ImguiHelper::Draw();

	//�`����I����
	PostEffect::PostDrawScene();
}