#include "Title.h"
#include "DX12Util.h"
#include "GameUtility.h"
#include "Input.h"
#include "SceneManager.h"
#include "Easing.h"
#include "FPSManager.h"
#include "GameSound.h"
#include "Encorder.h"

#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "BreakFloor.h"

using namespace DX12Library;

const float Title::EFFECT_ACCEL = 0.005f;

Title::Title()
{
	//���f��
	modelBG.CreateFromOBJ(MODEL_DIR + "Sky/Sky.obj");
	//�X�v���C�g
	sprTextTitle.Initialize();
	sprTextTitle.SetTexture(TEX_DIR_TITLE + "Text_Title.png");
	sprTextClick.Initialize();
	sprTextClick.SetTexture(TEX_DIR_TITLE + "Text_Click.png");
	sprBlack.Initialize();
	sprBlack.SetTexture(TEX_DIR_UTIL + "Black1280x720.png");
	sprWhite.Initialize();
	sprWhite.SetTexture(TEX_DIR_UTIL + "White1280x720.png");
	sprAttention.Initialize();
	sprAttention.SetTexture(TEX_DIR_TITLE + "Attention.png");
	//�p�[�e�B�N��
	for (int i = 0; i < _countof(particleSquare); i++) {
		particleSquare[i].LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Square.png");
		particleSquare[i].Initialize();
		particleSquare[i].SetColor(GameUtility::COLOR_VALUE[i]);
		particleSquare[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}
	for (int i = 0; i < _countof(particleTriangle); i++) {
		particleTriangle[i].LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Triangle.png");
		particleTriangle[i].Initialize();
		particleTriangle[i].SetColor(GameUtility::COLOR_VALUE[i]);
		particleTriangle[i].SetBlendMode(PARTICLE_BLENDMODE_ADD);
	}
	particleBreak.LoadTexture(Encorder::StrToWstr(TEX_DIR_UTIL) + L"Particle/Break.png");
	particleBreak.Initialize();
	particleBreak.SetColor({ 0.71f,0.47f, 0.2f, 1 });
	particleBreak.SetBlendMode(PARTICLE_BLENDMODE_ADD);
}

Title::~Title()
{
	effectObjects.clear();
}

void Title::Initialize()
{
	Object3D::SetMatrixOrthographicLH(1280 * 0.2f, 720 * 0.2f, 1.0f, 150.0f);

	//�J����������
	camera.Initialize();
	camera.SetPositionAndDistance({ 0,150, -50 }, 15.0f);
	camera.SetRotation(0, 0, 0);
	//�J�������Z�b�g
	DX12Util::SetCameraAll(&camera);

	//���C�g������
	light.Initialize();
	light.SetLightDir({ 0.5f,-1,0.5f });
	light.SetLightColor({ 1,1,1 });
	light.SetLightTarget({ 0,0,0 });
	light.CalcLightPos(50.0f);
	//���C�g���Z�b�g
	Object3D::SetLight(&light);

	//�O�i(��)�͍ŏ�������
	sprBlack.SetColor({ 1,1,1,0 });

	//�e�L�X�g
	sprTextTitle.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprTextTitle.GetTexSize().x * 0.5f, 150 });
	sprTextClick.SetPosition({ DX12Util::GetWindowWidth() * 0.5f - sprTextClick.GetTexSize().x * 0.5f, 450 });

	//�w�i�I�u�W�F�N�g������
	objBG.Initialize();
	objBG.SetObjModel(&modelBG);
	objBG.SetScale(5, 5, 5);
	objBG.SetDrawShadowToMyself(false);

	//�^�C�}�[�Z�b�g
	firstEffectTimer.SetTimer(0, 6000);
	firstEffectTimer.Start();
	sceneChangeTimer.SetTimer(0, 2000);
	clickAlphaTimer.SetTimer(0, 3000, true);
	clickAlphaTimer.Start();
	addEffectTimer.SetTimer(0, 1);
}

void Title::Update()
{
	UpdateTimer();
	UpdateInput();
	UpdateBG();
	UpdateEffectObjects();
	UpdateTextTex();
	UpdateAttention();
	UpdateFG();
	UpdateSound();
	//�T�E���h�X�V
	GameSound::Update();
}

void Title::Draw()
{
	DrawBG();
	DrawEffectObjects();
	DrawTextTex();
	DrawFG();
	DrawAttention();
}

void Title::UpdateInput()
{
	if (Mouse::IsMouseButtonRelease(LEFT)) {
		//���N���b�N�Ń^�C�g����
		if (firstEffectTimer.GetNowTime() < 4500) {
			//�����Ȃ�n�߂鎞��
			firstEffectTimer.SetNowTime(4500);
		}
		//���N���b�N�ŃX�e�[�W�Z���N�g��
		else if (firstEffectTimer.GetIsEnd() && sceneChangeTimer.GetIsStart() == false) {
			//�V�[���`�F���W�^�C�}�[�J�n
			sceneChangeTimer.Start();
			//���ʉ��炷
			GameSound::Play(L"GameStart");
		}
	}

	//�V�[���`�F���W�^�C�}�[�I���Ŏ��̃V�[���Ɉڂ�
	if (sceneChangeTimer.GetIsEnd()) {
		//�I�u�W�F�N�g�폜
		effectObjects.clear();
		//�V�[���`�F���W
		SceneManager::ChangeScene("StageSelect");
	}
}

void Title::UpdateTimer()
{
	//�J�����o�I�������G�t�F�N�g�^�C�}�[�J�n
	//(�Ȍ��addEffectTimer�I����ɐV���������_���ɊJ�n�����)
	//�G�t�F�N�g�ǉ�
	if (firstEffectTimer.GetIsEnd() && addEffectTimer.GetIsStart() == false) {
		AddEffectObject();
		//�^�C�}�[�Z�b�g
		SetAndStartAddEffectTimer();
	}
}

void Title::UpdateTextTex()
{
	//�uClick�v�̃e�N�X�`������
	if (sceneChangeTimer.GetIsStart()) {
		//�_��
		if (sceneChangeTimer.GetNowTime() < 500) {
			int alpha = sceneChangeTimer.GetNowTime() * 0.02;
			alpha = alpha % 2;
			sprTextClick.SetColor({ 1,1,1,(float)alpha });
		}
		//�ʏ�`��
		else {
			sprTextClick.SetColor({ 1,1,1,1 });
		}
	}
	//�������_��
	else {
		float digrees = (float)clickAlphaTimer.GetNowTime() * 180 / clickAlphaTimer.GetEndTime();
		float alpha = sin(digrees * PI / 180);
		sprTextClick.SetColor({ 1,1,1,alpha });
	}
}

void Title::UpdateAttention()
{
	//�C���z�������ق���������`�̕\���̓����x�X�V
	if (firstEffectTimer.GetNowTime() < 500) {
		float alpha = (((float)firstEffectTimer.GetNowTime()) / 500);
		sprAttention.SetColor({ 1,1,1,alpha });
		sprWhite.SetColor({ 1,1,1,1 });
	}
	else if (firstEffectTimer.GetNowTime() >= 4500) {
		float alpha = 1.0f - (((float)firstEffectTimer.GetNowTime() - 4500) / 500);
		sprAttention.SetColor({ 1,1,1,alpha });
		sprWhite.SetColor({ 1,1,1,alpha });
	}
	else {
		sprAttention.SetColor({ 1,1,1,1 });
		sprWhite.SetColor({ 1,1,1,1 });
	}
}

void Title::UpdateBG()
{
	//�V����]
	float rotY = 0.1f * FPSManager::GetMulAdjust60FPS();
	objBG.AddRotation(0, rotY, 0);
	objBG.Update();
}

void Title::UpdateFG()
{
	//�t�F�[�h�A�E�g
	if (sceneChangeTimer.GetNowTime() >= 1000) {
		float alpha = ((float)sceneChangeTimer.GetNowTime() - 1000.0f) / 500;
		sprBlack.SetColor({1,1,1,alpha});
	}
	else {
		if (firstEffectTimer.GetNowTime() >= 5500) {
			float alpha = 1.0f - (((float)firstEffectTimer.GetNowTime() - 5500) / (firstEffectTimer.GetEndTime() - 5500));
			sprBlack.SetColor({ 1,1,1,alpha });
		}
		else if (firstEffectTimer.GetNowTime() >= 4500) {
			sprBlack.SetColor({ 1,1,1,1 });
		}
	}
}

void Title::UpdateEffectObjects()
{
	//�G�t�F�N�g�I�u�W�F�N�g�X�V
	for (int i = 0; i < effectObjects.size(); i++) {
		//�d�͂��ۂ��̓K�p
		effectObjects[i]->velocity.y -= EFFECT_ACCEL;
		//�ʒu�X�V
		effectObjects[i]->position += effectObjects[i]->velocity * effectObjects[i]->moveSpeed;
		effectObjects[i]->object.SetPosition(effectObjects[i]->position);
		//��]�X�V
		Vector3 setRot = effectObjects[i]->object.GetRotation() + effectObjects[i]->addRotVelocity;
		effectObjects[i]->object.SetRotation(setRot);
		//�I�u�W�F�N�g�X�V
		effectObjects[i]->object.Update();
	}

	//�I�u�W�F�N�g�폜
	auto itr = effectObjects.begin();
	while (itr != effectObjects.end())
	{
		//�j��^�C�}�[���I�����Ă�����
		if ((*itr)->breakTimer.GetIsEnd())
		{
			//�p�[�e�B�N������
			AddParticle(*(*itr));
			//�G�t�F�N�g�I�u�W�F�N�g�폜
			itr = effectObjects.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	//�p�[�e�B�N���X�V
	for (int i = 0; i < _countof(particleSquare); i++) {
		particleSquare[i].Update();
	}
	for (int i = 0; i < _countof(particleTriangle); i++) {
		particleTriangle[i].Update();
	}
	particleBreak.Update();

}

void Title::UpdateSound()
{
	//�J���G�t�F�N�g�I�������BGM�Đ�
	if (firstEffectTimer.GetIsEnd() && GameSound::IsPlaying(L"Title") == false) {
		GameSound::Play(L"Title");
	}
}

void Title::DrawTextTex()
{
	sprTextTitle.DrawFG();
	sprTextClick.DrawFG();
}

void Title::DrawAttention()
{
	//�C���z���̒��ӕ`��
	if (firstEffectTimer.GetNowTime() < 5000) {
		sprAttention.DrawFG();
	}
}

void Title::DrawBG()
{
	if (firstEffectTimer.GetNowTime() >= 5000) {
		objBG.Draw();
	}
	else {
		sprWhite.DrawBG();
	}
}

void Title::DrawFG()
{
	sprBlack.DrawFG();
}

void Title::DrawEffectObjects()
{
	//�G�t�F�N�g�I�u�W�F�N�g�`��
	for (int i = 0; i < effectObjects.size(); i++) {
		effectObjects[i]->object.Draw();
	}
	//�p�[�e�B�N���`��
	for (int i = 0; i < _countof(particleSquare); i++) {
		particleSquare[i].Draw();
	}
	for (int i = 0; i < _countof(particleTriangle); i++) {
		particleTriangle[i].Draw();
	}
	particleBreak.Draw();
}

void Title::AddEffectObject()
{
	//��ʊO����I�u�W�F�N�g�����ł��ăp�b�Ƃ͂����鉉�o

	EffectObject* newEffectObject = new EffectObject;
	//�I�u�W�F�N�g��3��ނɕ���
	newEffectObject->type = rand() % 3;

	//�I�u�W�F�N�g�������ƃ��f���Z�b�g
	newEffectObject->object.Initialize();

	//���ꂼ�ꃂ�f���Z�b�g
	if (newEffectObject->type == PARTICLE_TYPE_SQUARE) {
		newEffectObject->object.SetObjModel(SquareBlock::GetModel(1));	//�Ђъ��ꂽ�l�p���u���b�N�̃��f��
	}
	else if (newEffectObject->type == PARTICLE_TYPE_TRIANGLE) {
		newEffectObject->object.SetObjModel(TriangleBlock::GetModel(1));	//�Ђъ��ꂽ�O�p�̃u���b�N�̃��f��
	}
	else if (newEffectObject->type == PARTICLE_TYPE_BREAK) {
		newEffectObject->object.SetObjModel(BreakFloor::GetModel());	//���鏰�u���b�N�̃��f��
	}

	//���ʏ�̈ʒu�A�ړ��ʁA�X�s�[�h�Z�b�g
	int range = 100;
	newEffectObject->position = GetRandomOnUnitSpherePos() * range;
	//�����ŃI�u�W�F�N�g�ɂ����W��K�p���Ă���
	newEffectObject->object.SetPosition(newEffectObject->position);
	//���S(�̂�����Ə�)�ɋ߂������֌�����
	Vector3 center = GetRandomOnUnitSpherePos() * 30 + Vector3(0, 20, 0);
	newEffectObject->velocity = (center - newEffectObject->position).Normalize();
	newEffectObject->moveSpeed = 1.5f;
	//�F�Z�b�g
	newEffectObject->colorType = rand() % 5;
	//Type��Break�łȂ���΃I�u�W�F�N�g�ɂ����f
	if (newEffectObject->type != PARTICLE_TYPE_BREAK)
	newEffectObject->object.SetColor(GameUtility::COLOR_VALUE[newEffectObject->colorType]);
	//��]��ݒ�
	float speed = GetRandF(1, 10);//��]�X�s�[�h�͈��
	newEffectObject->addRotVelocity = GetRandomOnUnitSpherePos() * speed;
	//�e�͕`�斳��
	newEffectObject->object.SetDrawShadowToMyself(false);
	//�j��܂ł̃^�C�}�[�Z�b�g
	newEffectObject->breakTimer.SetTimer(0, rand() % 2000);
	newEffectObject->breakTimer.Start();

	//�ǉ�
	effectObjects.emplace_back(newEffectObject);
}

void Title::AddParticle(const EffectObject& effectObject)
{
	for (int i = 0; i < 20; i++) {
		//������Ƀ����_���Ŕ�΂�
		float x = (float)((rand() % 200 - 100) * 0.01f);
		float y = (float)((rand() % 100) * 0.01f);
		float z = (float)((rand() % 200 - 100) * 0.01f);
		Vector3 vel = Vector3(x, y, z).Normalize() * 0.75f * FPSManager::GetMulAdjust60FPS();
		Vector3 acc = Vector3(0, -0.01f, 0) * FPSManager::GetMulAdjust60FPS();;
		float startScale = 6.0f;
		float endScale = 0;

		//�^�C�v�ŕ�����
		if (effectObject.type == PARTICLE_TYPE_SQUARE) {
			particleSquare[effectObject.colorType].Add(1000, effectObject.position, vel, acc, startScale, endScale);
		}
		else if (effectObject.type == PARTICLE_TYPE_TRIANGLE) {
			particleTriangle[effectObject.colorType].Add(1000, effectObject.position, vel, acc, startScale, endScale);
		}
		else if (effectObject.type == PARTICLE_TYPE_BREAK) {
			particleBreak.Add(1000, effectObject.position, vel, acc, startScale, endScale);
		}
	}
}

void Title::SetAndStartAddEffectTimer()
{
	//�����_���Ƀ^�C�}�[�Z�b�g
	int end = rand() % 800;

	addEffectTimer.SetTimer(0, end);
	addEffectTimer.Start();
}

float Title::GetRandF(int min, int max)
{
	int sub = (max - min);
	float randf = (float)(rand() % sub);
	float result = randf - ((max - min) / 2);
	return result;
}

Vector3 Title::GetRandomOnUnitSpherePos()
{
	//�P�ʋ���̃����_���Ȉʒu�v�Z
	//�Q�l�T�C�g:https://ushiostarfish.hatenablog.com/entry/2018/07/13/005942

	float x1, x2, s;
	do {
		x1 = GetRandF(-100, 100) / 100;
		x2 = GetRandF(-100, 100) / 100;
		s = x1 * x1 + x2 * x2;
	} while (s >= 1.0);

	float twoSqrtOneMinusS = 2.0 * sqrt(max(1.0 - s, 0.0));

	return Vector3(x1 * twoSqrtOneMinusS, x2 * twoSqrtOneMinusS, 1.0 - 2.0 * s);
}
