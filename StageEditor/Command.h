#pragma once
#include "GameUtility.h"
#include "EditorStage.h"
#include "Timer.h"
#include <vector>

//�R�}���h�̎��
enum CommandType {
	COMMAND_TYPE_ADD,
	COMMAND_TYPE_DELETE,
	COMMAND_TYPE_STAGE_WIDTH,
	COMMAND_TYPE_STAGE_HEIGHT,
	COMMAND_TYPE_SET_STARTLANE
};

//�R�}���h���e
struct CommandDetail {
	int commandType = -1;		//���
	int args1 = -1;				//Add,Delete...x���W ���̂ق��͕ύX�O�̈ʒu��傫��
	int args2 = -1;				//Add,Delete...y���W ���̂ق��͕ύX��̈ʒu��傫��
	int objectType = -1;		//�u���b�N������
	int blockFloorType = -1;	//�u���b�Nor���̎��
	int breakupCount = -1;		//�j��\�܂ł̉�
	int colorType = -1;			//�F�̎��

	//�ꕔ��r���Z�̃I�y���[�^
	bool operator == (const CommandDetail& d)const {
		bool b1 = commandType == d.commandType;
		bool b2 = args1 == d.args1;
		bool b3 = args2 == d.args2;
		bool b4 = objectType == d.objectType;
		bool b5 = blockFloorType == d.blockFloorType;
		bool b6 = breakupCount == d.breakupCount;
		bool b7 = colorType == d.colorType;

		return b1 && b2 && b3 && b4 && b5 && b6 && b7;
	}

	//�ꕔ��r���Z�̃I�y���[�^
	bool operator != (const CommandDetail& d)const {
		bool b1 = commandType != d.commandType;
		bool b2 = args1 != d.args1;
		bool b3 = args2 != d.args2;
		bool b4 = objectType != d.objectType;
		bool b5 = blockFloorType != d.blockFloorType;
		bool b6 = breakupCount != d.breakupCount;
		bool b7 = colorType != d.colorType;

		return b1 || b2 || b3 || b4 || b5 || b6 || b7;
	}
};

//�R�}���h�Ǘ��A�K�p�N���X
class Command
{
private:
	//�R�}���h�i�[�R���e�i
	static std::vector<CommandDetail> commands;
	//�ҏW����X�e�[�W�̃|�C���^
	static EditorStage* pStage;
	//���݂̃R�}���h�ʒu
	static int commandPos;
	//�������^�C�}�[
	static DX12Library::Timer longPushTimer;

public:
	/// <summary>
	/// ������
	/// </summary>
	static void Initialize();

	/// <summary>
	/// �X�V�@Ctrl+Z�̓K�p�Ȃǂ͂���
	/// </summary>
	static void Update();

	/// <summary>
	/// �R�}���h�ǉ�
	/// </summary>
	/// <param name="commandDetail"></param>
	static void AddCommand(const CommandDetail& commandDetail);

	/// <summary>
	/// �ҏW����X�e�[�W�̃|�C���^�Z�b�g
	/// </summary>
	/// <param name="pStage">�X�e�[�W�̃|�C���^</param>
	static void SetPStage(EditorStage* pStage) { Command::pStage = pStage; }

private:
	/// <summary>
	/// �^�C�}�[�X�V
	/// </summary>
	static void UpdateTimer();

	/// <summary>
	/// Ctrl + Z�������ꂽ����Ԃ�
	/// </summary>
	/// <returns>Ctrl + Z�������ꂽ��</returns>
	static bool IsUndo();

	/// <summary>
	/// Ctrl + Y�������ꂽ����Ԃ�
	/// </summary>
	/// <returns>Ctrl + Y�������ꂽ��</returns>
	static bool IsRedo();

	/// <summary>
	/// �A���h�D��K�p
	/// </summary>
	static void ApplyUndo();

	/// <summary>
	/// ���h�D��K�p
	/// </summary>
	static void ApplyRedo();

	/// <summary>
	/// �R�}���h���s
	/// </summary>
	/// <param name="commandDetail"></param>
	static void ApplyCommand(const CommandDetail& commandDetail);

	/// <summary>
	/// ���͂��ꂽ�R�}���h�̋t����̃R�}���h��Ԃ� �A���h�D�p
	/// </summary>
	/// <param name="src">���R�}���h</param>
	/// <returns>���R�}���h�̋t����</returns>
	static CommandDetail GetReverseCommand(const CommandDetail& src);
};

