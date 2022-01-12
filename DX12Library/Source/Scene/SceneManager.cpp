#include <assert.h>
#include "Input.h"
#include "SceneManager.h"
#include "ImguiHelper.h"
#include "PostEffect.h"
#include "RenderText.h"
#include "DebugText.h"
#include "InstancingObject.h"

//�ÓI�����o�ϐ��̎���
std::string SceneManager::nowScene;
std::string SceneManager::nextScene = "";
std::unordered_map<std::string, Scene*> SceneManager::scenes;

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
	for (auto itr = scenes.begin(); itr != scenes.end(); itr++)
	{
		delete itr->second;
	}

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

	scenes[nowScene]->Update();
}

void SceneManager::Draw()
{
	//���X�g�ɒǉ�
	scenes[nowScene]->Draw();

	//�f�o�b�O�e�L�X�g�̃X�v���C�g�O�i�`��o�^
	DebugText::DrawAll();

	//�܂��[�x�l�e�N�X�`����������
	Object3D::WriteDepthTex();

	//���Ƀ|�X�g�G�t�F�N�g�p�e�N�X�`���ɕ`��
	PostEffect::PreDrawScene();
	//�w�i�X�v���C�g�`��
	Sprite::DrawAllBG();

	//�w�i�e�L�X�g�`��
	RenderText::DrawAllBG();

	//�[�x�o�b�t�@�N���A
	DX12Util::ClearDepthBuffer(false);

	//�I�u�W�F�N�g�`��
	Object3D::DrawAll();

	//�O�i�X�v���C�g�`��
	Sprite::DrawAllFG();

	//�O�i�e�L�X�g�`��
	RenderText::DrawAllFG();

	//Imgui�`��
	ImguiHelper::Draw();

	//�`����I����
	PostEffect::PostDrawScene();
}