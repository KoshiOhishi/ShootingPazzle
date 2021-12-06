#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "Light.h"
#include "Camera.h"
#include "Object3D.h"
#include "TouchableObject.h"
#include "ObjModel.h"
#include "Timer.h"
#include "Sound.h"
#include "RenderText.h"
#include "CollisionManager.h"

#include "MyBullet.h"
#include "EnemyBullet.h"
#include "SquareBlock.h"
#include "TriangleBlock.h"
#include "Stage.h"

enum EditorMode {
	MODE_ADD,
	MODE_DELETE
};

class Editor :
	public Scene
{
private:
	//���C�g
	Light light;
	//�J����
	Camera camera;
	//�^�C�}�[
	Timer timer;
	//�X�e�[�W
	Stage stage;


	//Editor
	//���[�h
	int mode = MODE_ADD;
	int blockType = BLOCKTYPE_SQUARE;
	int shapeType = SHAPETYPE_NO_LEFTTOP;
	//���݂̃J�[�\���ʒu
	StagePos nowCursolPos;

	//�\���p�I�u�W�F�N�g
	SquareBlock squareBlock;
	TriangleBlock triangleBlock[4];

	//SaveLoad�p���O�i�[
	std::string ioname;

	//�������͒���
	bool isEnteringIOName = false;

public:
	Editor(){}
	~Editor(){}
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void UpdateEdit();

	void UpdateAdd();
	void UpdateDelete();
	void UpdateObject();

	void DrawEdit();

	void DrawBlock();

	void Save();

	void Load();

	void UpdateImgui();

	/// <summary>
	/// ���݂̃X�e�[�W��̃J�[�\���̈ʒu���擾
	/// </summary>
	void CalcNowCursolPos();
};