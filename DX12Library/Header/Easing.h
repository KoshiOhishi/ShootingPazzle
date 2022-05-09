#pragma once
#include <string>
#include "Timer.h"

//�Q�l:https://easings.net/ja

namespace DX12Library {

	enum EaseName {
		EASE_LINE,
		EASE_INSINE,
		EASE_OUTSINE,
		EASE_INOUTSINE,
		EASE_INQUAD,
		EASE_OUTQUAD,
		EASE_INOUTQUAD,
		EASE_INCUBIC,
		EASE_OUTCUBIC,
		EASE_INOUTCUBIC,
		EASE_INQUART,
		EASE_OUTQUART,
		EASE_INOUTQUART,
		EASE_INQUINT,
		EASE_OUTQUINT,
		EASE_INOUTQUINT,
		EASE_INEXPO,
		EASE_OUTEXPO,
		EASE_INOUTEXPO,
		EASE_INCIRC,
		EASE_OUTCIRC,
		EASE_INOUTCIRC,
		EASE_INBACK,
		EASE_OUTBACK,
		EASE_INOUTBACK,
		EASE_INELASTIC,
		EASE_OUTELASTIC,
		EASE_INOUTELASTIC,
		EASE_INBOUNCE,
		EASE_OUTBOUNCE,
		EASE_INOUTBOUNCE,
	};

	class Easing
	{

		static const float PI;

	private:
		static const double c1;
		static const double c2;
		static const double c3;
		static const double c4;
		static const double c5;
		static const double n1;
		static const double d1;

		/// <summary>
		/// �y�ʉ�pow ���R����̂Ƃ��͂��������g������
		/// </summary>
		/// <param name="x">�l</param>
		/// <param name="t">�ݏ搔</param>
		/// <returns>����</returns>
		static double Pow(double x, int t);

	public:
		/// <summary>
		/// �C�[�W���O���������l���擾
		/// </summary>
		/// <param name="type">�C�[�W���O�̎��</param>
		/// <param name="start">t=0�̎��̒l</param>
		/// <param name="end">t=1�̎��̒l</param>
		/// <param name="timer">�^�C�}�[ ���J�n-�I����0���傫�����邱��</param>
		/// <param name="startTime">t=0�̎��̎��� ������O��0�Œ�</param>
		/// <param name="endTime">t=1�̎��̎��ԁ@����ȍ~��1�Œ�</param>
		/// <returns>�C�[�W���O���������l</returns>
		static double GetEaseValue(int type, double start, double end, const Timer& timer, double startTime = -1, double endTime = -1);

		/// <summary>
		/// �C�[�W���O���������l���擾
		/// </summary>
		/// <param name="type">�C�[�W���O�̎��</param>
		/// <param name="start">t=0�̎��̒l</param>
		/// <param name="end">t=1�̎��̒l</param>
		/// <param name="t">0�`1�̊Ԃ̐��l</param>
		/// <returns>�C�[�W���O���������l</returns>
		static double GetEaseValue(int type, double start, double end, double t);

		static const std::string& GetFuncName(int type);
	};

}