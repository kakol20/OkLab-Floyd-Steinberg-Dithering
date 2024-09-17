#include "OkLab.h"

#include "../maths/Maths.hpp"

OkLab OkLab::sRGBtoOkLab(const sRGB& srgb) {
	double l1 = srgb.GetR();
	double a1 = srgb.GetG();
	double b1 = srgb.GetB();

	// to reduce floating point error
	if (l1 == 1. && a1 == 1. && b1 == 1.) return OkLab(1., 0., 0.);
	if (l1 == 0. && a1 == 0. && b1 == 0.) return OkLab(0., 0., 0.);

	const double scalar = 387916. / 30017.;
	const double limit = 11. / 280.;

	if (l1 == a1 && l1 == b1) {
		// if graycale - can skip some conversions

		// to Linear RGB
		l1 = l1 <= limit ? l1 / scalar : std::pow((l1 + 0.055) / 1.055, 2.4);

		// to LMS - can skip "to Linear LMS" conversion
		l1 = std::cbrt(l1);

		// can skip "to OkLab" conversion
		return OkLab(l1, 0., 0.);
	} else {
		// to Linear RGB
		l1 = l1 <= limit ? l1 / scalar : std::pow((l1 + 0.055) / 1.055, 2.4);
		a1 = a1 <= limit ? a1 / scalar : std::pow((a1 + 0.055) / 1.055, 2.4);
		b1 = b1 <= limit ? b1 / scalar : std::pow((b1 + 0.055) / 1.055, 2.4);

		// to Linear LMS
		double l2 = 0.412221470800 * l1 + 0.536332536300 * a1 + 0.051445992900 * b1;
		double a2 = 0.211903498234 * l1 + 0.680699545133 * a1 + 0.107396956633 * b1;
		double b2 = 0.088302461900 * l1 + 0.281718837600 * a1 + 0.629978700500 * b1;

		// to LMS
		l1 = std::cbrt(l2);
		a1 = std::cbrt(a2);
		b1 = std::cbrt(b2);

		// to OkLab
		l2 = 0.210454257467 * l1 + 0.793617787167 * a1 - 0.004072044634 * b1;
		a2 = 1.977998495100 * l1 - 2.428592205000 * a1 + 0.450593709900 * b1;
		b2 = 0.025904024666 * l1 + 0.782771753767 * a1 - 0.808675778433 * b1;

		return OkLab(l2, a2, b2);
	}
}

sRGB OkLab::OkLabtosRGB(const OkLab& oklab) {
	// to reduce floating point error
	if (oklab.GetL() == 1.) return sRGB(1., 1., 1.);
	if (oklab.GetL() == 0.) return sRGB(0., 0., 0.);

	double r1 = oklab.GetL();
	double g1 = oklab.GetA();
	double b1 = oklab.GetB();

	const double scalar = 387916. / 30017.;
	const double limit = 285. / 93752.;

	if (g1 == 0. && b1 == 0.) {
		// if graycale - can skip some conversions

		// to Linear LMS - can skip "to LMS" conversion
		r1 = r1 * r1 * r1;

		// to sRGB - can skip "to Linear RGB" conversion
		r1 = r1 <= limit ? scalar * r1 : (Maths::NRoot(r1, 2.4) * 1.055) - 0.055;

		return sRGB(r1, r1, r1);
	} else {
		// to LMS
		double r2 = r1 + 0.396337792278 * g1 + 0.215803757471 * b1;
		double g2 = r1 - 0.105561342920 * g1 - 0.063854171399 * b1;
		double b2 = r1 - 0.089484185764 * g1 - 1.291485517099 * b1;

		// to Linear LMS
		r1 = r2 * r2 * r2;
		g1 = g2 * g2 * g2;
		b1 = b2 * b2 * b2;

		// to Linear RGB
		r2 =  4.076741661667 * r1 - 3.307711590572 * g1 + 0.230969928905 * b1;
		g2 = -1.268438004344 * r1 + 2.609757400792 * g1 - 0.341319396448 * b1;
		b2 = -0.004196086474 * r1 - 0.703418614494 * g1 + 1.707614700968 * b1;

		// to sRGB
		r2 = r2 <= limit ? scalar * r2 : (Maths::NRoot(r2, 2.4) * 1.055) - 0.055;
		g2 = g2 <= limit ? scalar * g2 : (Maths::NRoot(g2, 2.4) * 1.055) - 0.055;
		b2 = b2 <= limit ? scalar * b2 : (Maths::NRoot(b2, 2.4) * 1.055) - 0.055;
		return sRGB(r2, g2, b2);
	}
}

OkLab& OkLab::operator/=(const OkLab& other) {
	ColorSpace lhs(other);
	ColorSpace::operator/=(lhs);
	return *this;
}

OkLab& OkLab::operator*=(const OkLab& other) {
	ColorSpace lhs(other);
	ColorSpace::operator*=(lhs);
	return *this;
}

OkLab& OkLab::operator+=(const OkLab& other) {
	ColorSpace lhs(other);
	ColorSpace::operator+=(lhs);
	return *this;
}

OkLab& OkLab::operator-=(const OkLab& other) {
	ColorSpace lhs(other);
	ColorSpace::operator-=(lhs);
	return *this;
}

OkLab& OkLab::operator*=(const double scalar) {
	ColorSpace::operator*=(scalar);
	return *this;
}

bool OkLab::IsInsidesRGB() const {
	sRGB current = OkLab::OkLabtosRGB(*this);
	return current.IsInside();
}

double OkLab::Distance(const OkLab& lab1, const OkLab& lab2, const bool lightMode) {
	if (lightMode) {
		return std::abs(lab1.GetL() - lab2.GetL());
	} else {
		const OkLab delta = lab1 - lab2;
		return std::sqrt(delta.GetL() * delta.GetL() + delta.GetA() * delta.GetA() + delta.GetB() * delta.GetB());
	}
	return 0.0;
}