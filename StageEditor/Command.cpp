#include "Command.h"
#include "Input.h"
#include "DebugText.h"

//�R�}���h�i�[�R���e�i
std::vector<CommandDetail> Command::commands;
//�ҏW����X�e�[�W�̃|�C���^
EditorStage* Command::pStage;
//���݂̃R�}���h�ʒu
int Command::commandPos = -1;
//�������^�C�}�[
Timer Command::longPushTimer;

void Command::Initialize()
{
	//�R�}���h�S�N���A
	commands.clear();
	commandPos = -1;

	//�^�C�}�[������
	longPushTimer.SetTimer(0, 300);
}

void Command::Update()
{
	//�^�C�}�[�X�V
	UpdateTimer();

	//�A���h�D����
	if (IsUndo()) {
		//�A���h�D�K�p
		ApplyUndo();
	}

	//���h�D����
	if (IsRedo()) {
		//���h�D�K�p
		ApplyRedo();
	}
}

void Command::AddCommand(const CommandDetail& commandDetail)
{

	//�R�}���h�̈ʒu�������Ȃ�
	if (commandPos == commands.size() - 1) {
		//�������e�̃R�}���h�����ɖ����ɑ��݂��Ă�����ǉ����Ȃ�
		if (commands.size() > 0 && commandDetail == commands[commands.size() - 1]) {
			return;
		}
	}
	//�R�}���h�̈ʒu�������łȂ��Ȃ�
	else {
		//��������R�}���h�̈ʒu�܂ł̐��폜
		int count = commands.size() - 1 - commandPos;
		for (int i = 0; i < count; i++) {
			commands.pop_back();
		}
	}

	//�ǉ�
	commands.emplace_back(commandDetail);

	//�R�}���h�ʒu�𖖔���
	commandPos = commands.size() - 1;
}

void Command::UpdateTimer()
{
	longPushTimer.Update();

	//�A���h�D/���h�D�L�[��������Ă�����^�C�}�[�X�^�[�g
	if ((Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && 
		(Keyboard::IsKeyPush(DIK_Z) || Keyboard::IsKeyPush(DIK_Y))) {
		if (longPushTimer.GetNowTime() <= longPushTimer.GetStartTime()) {
			longPushTimer.Start();
		}
	}
	else {
		longPushTimer.Reset();
	}
}

bool Command::IsUndo()
{
	//����������Ă��邩�ŏ����𕪂���
	if (longPushTimer.GetIsEnd()) {
		//���E�ǂ��炩��Ctrl��������Ă��āAZ��������Ă��邩
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyPush(DIK_Z);
	}
	else {
		//���E�ǂ��炩��Ctrl��������Ă��āAZ�������ꂽ�u�Ԃ�
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyTrigger(DIK_Z);
	}
}

bool Command::IsRedo()
{
	//����������Ă��邩�ŏ����𕪂���
	if (longPushTimer.GetIsEnd()) {
		//���E�ǂ��炩��Ctrl��������Ă��āAY��������Ă��邩
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyPush(DIK_Y);
	}
	else {
		//���E�ǂ��炩��Ctrl��������Ă��āAY�������ꂽ�u�Ԃ�
		return (Keyboard::IsKeyPush(DIK_LCONTROL) || Keyboard::IsKeyPush(DIK_RCONTROL)) && Keyboard::IsKeyTrigger(DIK_Y);
	}
}

void Command::ApplyUndo()
{
	//�R�}���h�ʒu��-1����������s����
	if (commandPos == -1) {
		return;
	}

	//�R�}���h�ʒu�̋t�R�}���h�����s
	ApplyCommand(GetReverseCommand(commands[commandPos]));
	//�R�}���h�ʒu��1�O��
	commandPos--;
}

void Command::ApplyRedo()
{
	//�R�}���h�ʒu����������������s�Ȃ�
	if (commandPos == commands.size() - 1) {
		return;
	}

	//�R�}���h�ʒu��1����
	commandPos++;

	//�R�}���h�ʒu�̃R�}���h�����s
	ApplyCommand(commands[commandPos]);
}

void Command::ApplyCommand(const CommandDetail& commandDetail)
{
	//�ʒu
	StageVec2 pos = { (unsigned short)commandDetail.args1, (unsigned short)commandDetail.args2 };

	//��ޕʂɃR�}���h���s
	switch (commandDetail.commandType) {

	case COMMAND_TYPE_ADD:
		//��ޕʂɒǉ�
		if (commandDetail.objectType == OBJECTTYPE_BLOCK) {
			pStage->AddBlock(pos, commandDetail.blockFloorType, commandDetail.breakupCount, commandDetail.colorType, false);//�R�}���h�ǉ�����
		}
		else if (commandDetail.objectType == OBJECTTYPE_FLOOR) {
			pStage->AddFloor(pos, commandDetail.blockFloorType, false);//�R�}���h�ǉ�����
		}
		break;

	case COMMAND_TYPE_DELETE:
		//��ޕʂɍ폜
		if (commandDetail.objectType == OBJECTTYPE_BLOCK) {
			pStage->DeleteBlock(pos, false);//�R�}���h�ǉ�����
		}
		else if (commandDetail.objectType == OBJECTTYPE_FLOOR) {
			pStage->DeleteFloor(pos, false);//�R�}���h�ǉ�����
		}
		break;

	case COMMAND_TYPE_STAGE_WIDTH:
		//�ύX��̒l��K�p
		pStage->stageSize.x = commandDetail.args2;
		break;
	case COMMAND_TYPE_STAGE_HEIGHT:
		//�ύX��̒l��K�p
		pStage->stageSize.y = commandDetail.args2;
		break;
	case COMMAND_TYPE_SET_STARTLANE:
		pStage->startLaneZ = commandDetail.args2;
		break;
	}
}

CommandDetail Command::GetReverseCommand(const CommandDetail& src)
{
	CommandDetail result = {};

	//�R�}���h�̎�ނ̋t��ݒ�
	switch (src.commandType) {

	//�ǉ����폜
	case COMMAND_TYPE_ADD:
		result.commandType = COMMAND_TYPE_DELETE;
		break;

	//�폜���ǉ�
	case COMMAND_TYPE_DELETE:
		result.commandType = COMMAND_TYPE_ADD;
		break;

	//���̂ق��͌��Ɠ������
	case COMMAND_TYPE_STAGE_WIDTH:
	case COMMAND_TYPE_STAGE_HEIGHT:
	case COMMAND_TYPE_SET_STARTLANE:
		result.commandType = src.commandType;
		break;
	}

	//�R�}���h�̓��e�̋t�ݒ�
	switch (result.commandType) {

	//�ǉ��A�폜�͈ʒu�Ȃ̂ł��̂܂܃R�s�[
	case COMMAND_TYPE_ADD:
	case COMMAND_TYPE_DELETE:
		result.args1 = src.args1;
		result.args2 = src.args2;
		break;

	//���̂ق��͕ύX�O�ƕύX��̒l�Ȃ̂œ���ւ���
	case COMMAND_TYPE_STAGE_WIDTH:
	case COMMAND_TYPE_STAGE_HEIGHT:
	case COMMAND_TYPE_SET_STARTLANE:
		result.args1 = src.args2;
		result.args2 = src.args1;
		break;
	}

	//(�ǉ��ƍ폜�̂�)�I�u�W�F�N�g�̎�ރR�s�[
	if (result.commandType == COMMAND_TYPE_ADD ||
		result.commandType == COMMAND_TYPE_DELETE) {
		result.objectType = src.objectType;
		result.blockFloorType = src.blockFloorType;
		result.breakupCount = src.breakupCount;
		result.colorType = src.colorType;
	}

	return result;
}
