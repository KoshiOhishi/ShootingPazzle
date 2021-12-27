#pragma once
#include "InstancingObject.h"
#include "ObjModel.h"
#include "Vector2.h"
#include "Vector3.h"
#include "CollisionPrimitive.h"
#include "GameUtility.h"
#include "Timer.h"
#include "Stage.h"

class Stage;

enum BlockColor
{
	BLOCK_COLOR_BLACK,
	BLOCK_COLOR_RED,
	BLOCK_COLOR_BLUE,
	BLOCK_COLOR_YELLOW,
	BLOCK_COLOR_GREEN
};

class BaseBlock
{
protected:
	//�C���X�^���V���O�`��I�u�W�F�N�g
	InstancingObject object;
	//�X�e�[�W�|�C���^
	static Stage* pStage;
	//���Ƃ̏Փ˔���p�J�v�Z��
	std::vector<Capsule> capsule;
	//�Փˌ�ɉ���u���b�N��
	bool isBreakable = false;
	//���Ɖ��񓖂���Ή��邩�@�����l0�Ŕj��s�\�u���b�N�ɂȂ�
	unsigned short breakupCount = 0;
	//�u���b�N�̐F
	int blockColor = BLOCK_COLOR_BLACK;

	//�o���G�t�F�N�g�̎��
	int firstEffectType = -1;
	//�o���G�t�F�N�g�̏I������
	double firstEffectEndTime = -1;
	//�N���A�G�t�F�N�g�̊J�n����
	double clearEffectStartTime = -1;

public:

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~BaseBlock() {};

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize(const StageVec2& pos, float sphereRadius) = 0;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �o���G�t�F�N�g���̍X�V
	/// </summary>
	virtual void UpdateFirstEffect(const Timer& timer);

	/// <summary>
	/// �N���A�G�t�F�N�g�̍X�V
	/// </summary>
	/// <param name="timer"></param>
	virtual void UpdateClearEffect(const Timer& timer);

	/// <summary>
	/// �����蔻��X�V
	/// </summary>
	virtual void UpdateCollision() = 0;

	/// <summary>
	/// �u���b�N�̎�ނ��擾����
	/// </summary>
	/// <returns>�u���b�N�̎��</returns>
	virtual std::string GetObjectType() = 0;

	/// <summary>
	/// ����܂ł̃J�E���g��1���炷 0�ɂȂ�����Breakup()���Ă�
	/// </summary>
	virtual void DecrementBreakupCount() = 0;

	/// <summary>
	/// �u���b�N����
	/// </summary>
	virtual void Breakup() = 0;

#pragma region Setter
	static void SetPStage(Stage* pStage) { BaseBlock::pStage = pStage; }

	/// <summary>
	/// ���W�Z�b�g (Y���W�Œ�)
	/// </summary>
	/// <param name="pos">�X�e�[�W��̍��W</param>
	void SetStagePos(const StageVec2& pos);

	/// <summary>
	/// ���W�Z�b�g�i��ɃG�t�F�N�g�p�j
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos) { object.SetPosition(pos); }

	/// <summary>
	/// ���邩�ǂ����̃t���O�Z�b�g
	/// </summary>
	/// <param name="isBreakable"></param>
	void SetBreakupCount(unsigned short breakupCount);

	/// <summary>
	/// �u���b�N�F�Z�b�g
	/// </summary>
	/// <param name="color">BLOCK_COLOR_...</param>
	void SetBlockColor(int blockColor);

#pragma endregion
#pragma region Getter

	const Vector3& GetPosition() { return object.GetPosition(); }

	/// <summary>
	/// �J�v�Z�������蔻����擾
	/// </summary>
	/// <returns>�J�v�Z�������蔻��</returns>
	const std::vector<Capsule> GetCapsule()const {
		return capsule;
	}

	/// <summary>
	/// ����u���b�N���ǂ���
	/// </summary>
	/// <returns></returns>
	bool IsBreakable() { return isBreakable; }

	/// <summary>
	/// ����܂ł̉񐔂�Ԃ�
	/// </summary>
	/// <returns></returns>
	unsigned short GetBreakupCount() {return breakupCount;}

	/// <summary>
	/// �u���b�N�̐F�擾
	/// </summary>
	/// <returns></returns>
	int GetBlockColor() { return blockColor; }

	/// <summary>
	/// �u���b�N�����Ă��邩�ǂ���
	/// </summary>
	/// <returns></returns>
	bool isBreakup() { return isBreakable && breakupCount <= 0; }

#pragma endregion
};

