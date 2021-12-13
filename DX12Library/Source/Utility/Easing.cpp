#include "Easing.h"
#include <cassert>
#include <cmath>

double Easing::Pow(double x, int t)
{
	double val = 1;
	for (int i = 0; i < t; i++) {
		val *= x;
	}

	return val;
}

void Easing::Update()
{
	paramTimer.Update();

	double x = (double)paramTimer.GetNowTime() / paramTimer.GetEndTime();

	switch (type) {

	case EASE_INSIDE:
		value = 1.0 - cos((x * PI) / 2.0);
		break;

	case EASE_OUTSINE:
		value = sin((x * PI) / 2.0);
		break;

	case EASE_INOUTSINE:
		value = -(cos(PI * x) - 1.0) / 2.0;
		break;

	case EASE_INQUAD:
		value = x * x;
		break;

	case EASE_OUTQUAD:
		value = 1.0 - (1.0 - x) * (1.0 - x);
		break;

	case EASE_INOUTQUAD:
		value = x < 0.5 ? 2.0 * x * x : 1.0 - Pow(-2.0 * x + 2, 2) / 2.0;
		break;

	case EASE_INCUBIC:
		value = x * x * x;
		break;

	case EASE_OUTCUBIC:
		value = 1.0 - Pow(1.0 - x, 3);
		break;

	case EASE_INOUTCUBIC:
		value = x < 0.5 ? 4.0 * x * x * x : 1.0 - Pow(-2.0 * x + 2.0, 3) / 2.0;
		break;

	case EASE_INQUART:
		value = x * x * x * x;
		break;

	case EASE_OUTQUART:
		value = 1.0 - Pow(1.0 - x, 4);
		break;

	case EASE_INOUTQUART:
		value = x < 0.5 ? 8.0 * x * x * x * x : 1.0 - Pow(-2.0 * x + 2.0, 4) / 2.0;
		break;

	case EASE_INQUINT:
		value = x * x * x * x * x;
		break;

	case EASE_OUTQUINT:
		value = 1.0 - Pow(1.0 - x, 5);
		break;

	case EASE_INOUTQUINT:
		value = x < 0.5 ? 16.0 * x * x * x * x * x : 1.0 - Pow(-2.0 * x + 2.0, 5) / 2.0;
		break;

	case EASE_INEXPO:
		value = x == 0 ? 0 : pow(2.0, 10.0 * x - 10.0);
		break;

	case EASE_OUTEXPO:
		value = x == 1 ? 1 : 1 - pow(2.0, -10.0 * x);
		break;

	case EASE_INOUTEXPO:
		value = x == 0
			? 0
			: x == 1
			? 1
			: x < 0.5 ? pow(2.0, 20.0 * x - 10.0) / 2.0
			: (2.0 - pow(2.0, -20.0 * x + 10.0)) / 2.0;
		break;

	case EASE_INCIRC:
		value = 1.0 - sqrt(1.0 - Pow(x, 2));
		break;

	case EASE_OUTCIRC:
		value = sqrt(1.0 - Pow(x - 1.0, 2));
		break;

	case EASE_INOUTCIRC:
		value = x < 0.5
			? (1.0 - sqrt(1.0 - Pow(2.0 * x, 2))) / 2.0
			: (sqrt(1.0 - Pow(-2.0 * x + 2.0, 2)) + 1.0) / 2.0;
		break;

	case EASE_INBACK:
		value = c3 * x * x * x - c1 * x * x;
		break;

	case EASE_OUTBACK:
		value = 1.0 + c3 * Pow(x - 1.0, 3) + c1 * Pow(x - 1.0, 2);
		break;

	case EASE_INOUTBACK:
		value = x < 0.5
			? (pow(2.0 * x, 2) * ((c2 + 1.0) * 2.0 * x - c2)) / 2.0
			: (pow(2.0 * x - 2.0, 2) * ((c2 + 1.0) * (x * 2.0 - 2.0) + c2) + 2.0) / 2.0;
		break;

	case EASE_INELASTIC:
		value = x == 0
			? 0
			: x == 1.0
			? 1.0
			: -pow(2.0, 10.0 * x - 10.0) * sin((x * 10.0 - 10.75) * c4);
		break;

	case EASE_OUTELASTIC:
		value = x == 0
			? 0
			: x == 1
			? 1
			: pow(2.0, -10.0 * x) * sin((x * 10.0 - 0.75) * c4) + 1.0;
		break;

	case EASE_INOUTELASTIC:
		value = x == 0
			? 0
			: x == 1
			? 1
			: x < 0.5
			? -(pow(2.0, 20.0 * x - 10.0) * sin((20.0 * x - 11.125) * c5)) / 2.0
			: (pow(2.0, -20.0 * x + 10.0) * sin((20.0 * x - 11.125) * c5)) / 2.0 + 1.0;
		break;

	case EASE_INBOUNCE:
		x = 1.0 - x;
		if (x < 1.0 / d1) {
			value = 1.0 - (n1 * x * x);
		}
		else if (x < 2.0 / d1) {
			value = 1.0 - (n1 * (x -= 1.5 / d1) * x + 0.75);
		}
		else if (x < 2.5 / d1) {
			value = 1.0 - (n1 * (x -= 2.25 / d1) * x + 0.9375);
		}
		else {
			value = 1.0 - (n1 * (x -= 2.625 / d1) * x + 0.984375);
		}
		break;

	case EASE_OUTBOUNCE:
		if (x < 1.0 / d1) {
			value = n1 * x * x;
		}
		else if (x < 2.0 / d1) {
			value = n1 * (x -= 1.5 / d1) * x + 0.75;
		}
		else if (x < 2.5 / d1) {
			value = n1 * (x -= 2.25 / d1) * x + 0.9375;
		}
		else {
			value = n1 * (x -= 2.625 / d1) * x + 0.984375;
		}
		break;

	case EASE_INOUTBOUNCE:
		if (x < 0.5) {
			x = 1.0 - 2.0 * x;
			if (x < 1.0 / d1) {
				value = (1.0 - (n1 * x * x)) / 2.0;
			}
			else if (x < 2.0 / d1) {
				value = (1.0 - (n1 * (x -= 1.5 / d1) * x + 0.75)) / 2.0;
			}
			else if (x < 2.5 / d1) {
				value = (1.0 - (n1 * (x -= 2.25 / d1) * x + 0.9375)) / 2.0;
			}
			else {
				value = (1.0 - (n1 * (x -= 2.625 / d1) * x + 0.984375)) / 2.0;
			}
		}
		else {
			x = 2.0 * x - 1.0;
			if (x < 1.0 / d1) {
				value = (1.0 + (n1 * x * x)) / 2.0;
			}
			else if (x < 2.0 / d1) {
				value = (1.0 + (n1 * (x -= 1.5 / d1) * x + 0.75)) / 2.0;
			}
			else if (x < 2.5 / d1) {
				value = (1.0 + (n1 * (x -= 2.25 / d1) * x + 0.9375)) / 2.0;
			}
			else {
				value = (1.0 + (n1 * (x -= 2.625 / d1) * x + 0.984375)) / 2.0;
			}
		}
		break;

	default:
		value = x;
		break;
	}

	double amount = end - start;
	value = start + amount * value;
}

void Easing::Set(double start, double end, int type, double takesTime)
{
	this->start = start;
	this->end = end;
	this->type = type;

	//範囲内かチェック
	if (this->type < -1 || this->type > 29) {
		assert(0);
		type = -1;
	}

	paramTimer.SetTimer(0, takesTime);
}

void Easing::Start()
{
	paramTimer.Start();
}

void Easing::Stop()
{
	paramTimer.Stop();
}

void Easing::Reset()
{
	paramTimer.Reset();
}

std::string Easing::GetFuncName(int type)
{
	if (type < -1 || type > 29) {
		return "NONE";
	}

	switch (type) {
	case EASE_LINE:
		return "EASE_LINE";

	case EASE_INSIDE:
		return "EASE_INSIDE";

	case EASE_OUTSINE:
		return "EASE_OUTSINE";

	case EASE_INOUTSINE:
		return "EASE_INOUTSINE";

	case EASE_INQUAD:
		return "EASE_INQUAD";

	case EASE_OUTQUAD:
		return "EASE_OUTQUAD";

	case EASE_INOUTQUAD:
		return "EASE_INOUTQUAD";

	case EASE_INCUBIC:
		return "EASE_INCUBIC";

	case EASE_OUTCUBIC:
		return "EASE_OUTCUBIC";

	case EASE_INOUTCUBIC:
		return "EASE_INOUTCUBIC";

	case EASE_INQUART:
		return "EASE_INQUART";

	case EASE_OUTQUART:
		return "EASE_OUTQUART";

	case EASE_INOUTQUART:
		return "EASE_INOUTQUART";

	case EASE_INQUINT:
		return "EASE_INQUINT";

	case EASE_OUTQUINT:
		return "EASE_OUTQUINT";

	case EASE_INOUTQUINT:
		return "EASE_INOUTQUINT";

	case EASE_INEXPO:
		return "EASE_INEXPO";

	case EASE_OUTEXPO:
		return "EASE_OUTEXPO";

	case EASE_INOUTEXPO:
		return "EASE_INOUTEXPO";

	case EASE_INCIRC:
		return "EASE_INCIRC";

	case EASE_OUTCIRC:
		return "EASE_OUTCIRC";

	case EASE_INOUTCIRC:
		return "EASE_INOUTCIRC";

	case EASE_INBACK:
		return "EASE_INBACK";

	case EASE_OUTBACK:
		return "EASE_OUTBACK";

	case EASE_INOUTBACK:
		return "EASE_INOUTBACK";

	case EASE_INELASTIC:
		return "EASE_INELASTIC";

	case EASE_OUTELASTIC:
		return "EASE_OUTELASTIC";

	case EASE_INOUTELASTIC:
		return "EASE_INOUTELASTIC";

	case EASE_INBOUNCE:
		return "EASE_INBOUNCE";

	case EASE_OUTBOUNCE:
		return "EASE_OUTBOUNCE";

	case EASE_INOUTBOUNCE:
		return "EASE_INOUTBOUNCE";
	}

}
