#include "OkLab.h"

#include "../maths/Maths.hpp"

OkLab OkLab::sRGBtoOkLab(const sRGB& srgb) {
	double l1 = srgb.GetR();
	double a1 = srgb.GetG();
	double b1 = srgb.GetB();

	// to reduce floating point error
	if (l1 == 1. && a1 == 1. && b1 == 1.) return OkLab(1., 0., 0.);
	if (l1 == 0. && a1 == 0. && b1 == 0.) return OkLab(0., 0., 0.);

	if (l1 == a1 && l1 == b1) {
		// if graycale - can skip some conversions

		// to Linear RGB
		l1 = l1 <= 0.04045 ? l1 / 12.92 : std::pow((l1 + 0.055) / 1.055, 2.4);

		// to LMS - can skip "to Linear LMS" conversion
		l1 = std::cbrt(l1);

		// can skip "to OkLab" conversion
		return OkLab(l1, 0., 0.);
	} else {
		// to Linear RGB
		l1 = l1 <= 0.04045 ? l1 / 12.92 : std::pow((l1 + 0.055) / 1.055, 2.4);
		a1 = a1 <= 0.04045 ? a1 / 12.92 : std::pow((a1 + 0.055) / 1.055, 2.4);
		b1 = b1 <= 0.04045 ? b1 / 12.92 : std::pow((b1 + 0.055) / 1.055, 2.4);

		// to Linear LMS
		double l2 = 0.41224204988807 * l1 + 0.53626162185168 * a1 + 0.05142804288870 * b1;
		double a2 = 0.21194297298929 * l1 + 0.68070218481804 * a1 + 0.10737408156507 * b1;
		double b2 = 0.08835888958899 * l1 + 0.28184744754987 * a1 + 0.63012965338243 * b1;

		// to LMS
		l1 = std::cbrt(l2);
		a1 = std::cbrt(a2);
		b1 = std::cbrt(b2);

		// to OkLab
		l2 = 0.21045425666795 * l1 + 0.79361779015852 * a1 - 0.00407204682647 * b1;
		a2 = 1.97799849510000 * l1 - 2.42859220500000 * a1 + 0.45059370990000 * b1;
		b2 = 0.02590402925006 * l1 + 0.78277173659806 * a1 - 0.80867576584811 * b1;

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

	if (g1 == 0. && b1 == 0.) {
		// if graycale - can skip some conversions

		// to Linear LMS - can skip "to LMS" conversion
		r1 = r1 * r1 * r1;

		// to sRGB - can skip "to Linear RGB" conversion
		r1 = r1 <= 0.00313058 ? 12.92 * r1 : (Maths::NRoot(r1, 2.4) * 1.055) - 0.055;

		return sRGB(r1, r1, r1);
	} else {
		// to LMS
		double r2 = r1 + 0.39633779217377 * g1 + 0.21580375806076 * b1;
		double g2 = r1 - 0.10556134232366 * g1 - 0.06385417477171 * b1;
		double b2 = r1 - 0.08948418209497 * g1 - 1.29148553786409 * b1;

		// to Linear LMS
		r1 = r2 * r2 * r2;
		g1 = g2 * g2 * g2;
		b1 = b2 * b2 * b2;

		// to Linear RGB
		r2 = 4.07653881638861 * r1 - 3.30709682773943 * g1 + 0.23082245163012 * b1;
		g2 = -1.26860625095165 * r1 + 2.60974767679763 * g1 - 0.34116363525495 * b1;
		b2 = -0.00419756377401 * r1 - 0.70356840947339 * g1 + 1.70720561792434 * b1;

		// to sRGB
		r2 = r2 <= 0.00313058 ? 12.92 * r2 : (Maths::NRoot(r2, 2.4) * 1.055) - 0.055;
		g2 = g2 <= 0.00313058 ? 12.92 * g2 : (Maths::NRoot(g2, 2.4) * 1.055) - 0.055;
		b2 = b2 <= 0.00313058 ? 12.92 * b2 : (Maths::NRoot(b2, 2.4) * 1.055) - 0.055;
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