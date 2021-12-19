#include "Easing.h"
#include <cassert>
#include <cmath>

const double Easing::c1 = 1.70158;
const double Easing::c2 = c1 * 1.525;
const double Easing::c3 = c1 + 1.0;
const double Easing::c4 = (2.0 * PI) / 3.0;
const double Easing::c5 = (2.0 * PI) / 4.5;
const double Easing::n1 = 7.5625;
const double Easing::d1 = 2.75;

double Easing::Pow(double x, int t)
{
	double val = 1;
	for (int i = 0; i < t; i++) {
		val *= x;
	}

	return val;
}

double Easing::GetEaseValue(int type, double start, double end, double t)
{
	//t‚ð0`1‚ÉƒNƒŠƒbƒv
	if (t < 0) {
		t = 0; 
	}
	else if (t > 1) {
		t = 1; 
	}

	double value = 0;

	switch (type) {

	case EASE_INSIDE:
		value = 1.0 - cos((t * PI) / 2.0);
		break;

	case EASE_OUTSINE:
		value = sin((t * PI) / 2.0);
		break;

	case EASE_INOUTSINE:
		value = -(cos(PI * t) - 1.0) / 2.0;
		break;

	case EASE_INQUAD:
		value = t * t;
		break;

	case EASE_OUTQUAD:
		value = 1.0 - (1.0 - t) * (1.0 - t);
		break;

	case EASE_INOUTQUAD:
		value = t < 0.5 ? 2.0 * t * t : 1.0 - Pow(-2.0 * t + 2, 2) / 2.0;
		break;

	case EASE_INCUBIC:
		value = t * t * t;
		break;

	case EASE_OUTCUBIC:
		value = 1.0 - Pow(1.0 - t, 3);
		break;

	case EASE_INOUTCUBIC:
		value = t < 0.5 ? 4.0 * t * t * t : 1.0 - Pow(-2.0 * t + 2.0, 3) / 2.0;
		break;

	case EASE_INQUART:
		value = t * t * t * t;
		break;

	case EASE_OUTQUART:
		value = 1.0 - Pow(1.0 - t, 4);
		break;

	case EASE_INOUTQUART:
		value = t < 0.5 ? 8.0 * t * t * t * t : 1.0 - Pow(-2.0 * t + 2.0, 4) / 2.0;
		break;

	case EASE_INQUINT:
		value = t * t * t * t * t;
		break;

	case EASE_OUTQUINT:
		value = 1.0 - Pow(1.0 - t, 5);
		break;

	case EASE_INOUTQUINT:
		value = t < 0.5 ? 16.0 * t * t * t * t * t : 1.0 - Pow(-2.0 * t + 2.0, 5) / 2.0;
		break;

	case EASE_INEXPO:
		value = t == 0 ? 0 : pow(2.0, 10.0 * t - 10.0);
		break;

	case EASE_OUTEXPO:
		value = t == 1 ? 1 : 1 - pow(2.0, -10.0 * t);
		break;

	case EASE_INOUTEXPO:
		value = t == 0
			? 0
			: t == 1
			? 1
			: t < 0.5 ? pow(2.0, 20.0 * t - 10.0) / 2.0
			: (2.0 - pow(2.0, -20.0 * t + 10.0)) / 2.0;
		break;

	case EASE_INCIRC:
		value = 1.0 - sqrt(1.0 - Pow(t, 2));
		break;

	case EASE_OUTCIRC:
		value = sqrt(1.0 - Pow(t - 1.0, 2));
		break;

	case EASE_INOUTCIRC:
		value = t < 0.5
			? (1.0 - sqrt(1.0 - Pow(2.0 * t, 2))) / 2.0
			: (sqrt(1.0 - Pow(-2.0 * t + 2.0, 2)) + 1.0) / 2.0;
		break;

	case EASE_INBACK:
		value = c3 * t * t * t - c1 * t * t;
		break;

	case EASE_OUTBACK:
		value = 1.0 + c3 * Pow(t - 1.0, 3) + c1 * Pow(t - 1.0, 2);
		break;

	case EASE_INOUTBACK:
		value = t < 0.5
			? (pow(2.0 * t, 2) * ((c2 + 1.0) * 2.0 * t - c2)) / 2.0
			: (pow(2.0 * t - 2.0, 2) * ((c2 + 1.0) * (t * 2.0 - 2.0) + c2) + 2.0) / 2.0;
		break;

	case EASE_INELASTIC:
		value = t == 0
			? 0
			: t == 1.0
			? 1.0
			: -pow(2.0, 10.0 * t - 10.0) * sin((t * 10.0 - 10.75) * c4);
		break;

	case EASE_OUTELASTIC:
		value = t == 0
			? 0
			: t == 1
			? 1
			: pow(2.0, -10.0 * t) * sin((t * 10.0 - 0.75) * c4) + 1.0;
		break;

	case EASE_INOUTELASTIC:
		value = t == 0
			? 0
			: t == 1
			? 1
			: t < 0.5
			? -(pow(2.0, 20.0 * t - 10.0) * sin((20.0 * t - 11.125) * c5)) / 2.0
			: (pow(2.0, -20.0 * t + 10.0) * sin((20.0 * t - 11.125) * c5)) / 2.0 + 1.0;
		break;

	case EASE_INBOUNCE:
		t = 1.0 - t;
		if (t < 1.0 / d1) {
			value = 1.0 - (n1 * t * t);
		}
		else if (t < 2.0 / d1) {
			value = 1.0 - (n1 * (t -= 1.5 / d1) * t + 0.75);
		}
		else if (t < 2.5 / d1) {
			value = 1.0 - (n1 * (t -= 2.25 / d1) * t + 0.9375);
		}
		else {
			value = 1.0 - (n1 * (t -= 2.625 / d1) * t + 0.984375);
		}
		break;

	case EASE_OUTBOUNCE:
		if (t < 1.0 / d1) {
			value = n1 * t * t;
		}
		else if (t < 2.0 / d1) {
			value = n1 * (t -= 1.5 / d1) * t + 0.75;
		}
		else if (t < 2.5 / d1) {
			value = n1 * (t -= 2.25 / d1) * t + 0.9375;
		}
		else {
			value = n1 * (t -= 2.625 / d1) * t + 0.984375;
		}
		break;

	case EASE_INOUTBOUNCE:
		if (t < 0.5) {
			t = 1.0 - 2.0 * t;
			if (t < 1.0 / d1) {
				value = (1.0 - (n1 * t * t)) / 2.0;
			}
			else if (t < 2.0 / d1) {
				value = (1.0 - (n1 * (t -= 1.5 / d1) * t + 0.75)) / 2.0;
			}
			else if (t < 2.5 / d1) {
				value = (1.0 - (n1 * (t -= 2.25 / d1) * t + 0.9375)) / 2.0;
			}
			else {
				value = (1.0 - (n1 * (t -= 2.625 / d1) * t + 0.984375)) / 2.0;
			}
		}
		else {
			t = 2.0 * t - 1.0;
			if (t < 1.0 / d1) {
				value = (1.0 + (n1 * t * t)) / 2.0;
			}
			else if (t < 2.0 / d1) {
				value = (1.0 + (n1 * (t -= 1.5 / d1) * t + 0.75)) / 2.0;
			}
			else if (t < 2.5 / d1) {
				value = (1.0 + (n1 * (t -= 2.25 / d1) * t + 0.9375)) / 2.0;
			}
			else {
				value = (1.0 + (n1 * (t -= 2.625 / d1) * t + 0.984375)) / 2.0;
			}
		}
		break;

	default:
		value = t;
		break;
	}

	double amount = end - start;
	value = start + amount * value;

	return value;
}

double Easing::GetEaseValue(int type, double start, double end, const Timer& timer, double startTime, double endTime)
{
	double n = (double)timer.GetNowTime();
	double s = (double)timer.GetStartTime();
	double e = (double)timer.GetEndTime();

	if (startTime != -1) {
		s = startTime;
	}
	if (endTime != -1) {
		e = endTime;
	}

	double a = n - s;
	double b = e - s;

	//ƒ[ƒœŽZ
	if (b == 0) { return 0; }

	double t = a / b;

	return GetEaseValue(type, start, end, t);
}

const std::string& Easing::GetFuncName(int type)
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
