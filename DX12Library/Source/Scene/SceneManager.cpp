#include <assert.h>
#include "Input.h"
#include "SceneManager.h"
#include "ImguiHelper.h"
#include "PostEffect.h"
#include "RenderText.h"
#include "DebugText.h"

//�ÓI�����o�ϐ��̎���
SceneManager::string SceneManager::nowScene;
SceneManager::vector<Scene*> SceneManager::scenes;
int SceneManager::sceneNum;

void SceneManager::AddScene(Scene* scene, const string& sceneName)
{
	scene->SetSceneName(sceneName);
	scenes.push_back(scene);
}

void SceneManager::SetScene(const string& sceneName)
{
	nowScene = sceneName;
	for (int i = 0; i < scenes.size(); i++)
	{
		//���݃V�[���̂ݓK�p
		if (scenes[i]->GetSceneName() != nowScene) continue;

		sceneNum = i;
		Initialize();
		return;
	}
}

void SceneManager::DeleteScene()
{
	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}

	scenes.clear();
}

void SceneManager::Initialize()
{
	scenes[sceneNum]->Initialize();
}

void SceneManager::Update()
{
	scenes[sceneNum]->Update();
}

void SceneManager::Draw()
{
	//���X�g�ɒǉ�
	scenes[sceneNum]->Draw();

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