#include "../maths/Maths.hpp"
#include "OkLCh.h"

OkLCh OkLCh::sRGBtoOkLCh(const sRGB& srgb) {
	double l1 = srgb.GetR();
	double c1 = srgb.GetG();
	double h1 = srgb.GetB();

	// to reduce floating point error
	if (l1 == 1. && c1 == 1. && h1 == 1.) return OkLCh(1., 0., 0.);
	if (l1 == 0. && c1 == 0. && h1 == 0.) return OkLCh(0., 0., 0.);

	const double scalar = 387916. / 30017.;
	const double limit = 11. / 280.;

	if (l1 == c1 && l1 == h1) {
		// if graycale - can skip some conversions

		// to Linear RGB
		l1 = l1 <= limit ? l1 / scalar : std::pow((l1 + 0.055) / 1.055, 2.4);

		// to LMS - can skip "to Linear LMS" conversion
		l1 = std::cbrt(l1);

		// can skip "to OkLab" & "to OkLCh" conversions
		return OkLCh(l1, 0., 0.);
	} else {
		// to Linear RGB
		l1 = l1 <= limit ? l1 / scalar : std::pow((l1 + 0.055) / 1.055, 2.4);
		c1 = c1 <= limit ? c1 / scalar : std::pow((c1 + 0.055) / 1.055, 2.4);
		h1 = h1 <= limit ? h1 / scalar : std::pow((h1 + 0.055) / 1.055, 2.4);

		// to Linear LMS
		double l2 = 0.412221470800 * l1 + 0.536332536300 * c1 + 0.051445992900 * h1;
		double c2 = 0.211903498234 * l1 + 0.680699545133 * c1 + 0.107396956633 * h1;
		double h2 = 0.088302461900 * l1 + 0.281718837600 * c1 + 0.629978700500 * h1;

		// to LMS
		//val.Cbrt()
		l1 = std::cbrt(l2);
		c1 = std::cbrt(c2);
		h1 = std::cbrt(h2);

		// to OkLab
		l2 = 0.210454257467 * l1 + 0.793617787167 * c1 - 0.004072044634 * h1;
		c2 = 1.977998495100 * l1 - 2.428592205000 * c1 + 0.450593709900 * h1;
		h2 = 0.025904024666 * l1 + 0.782771753767 * c1 - 0.808675778433 * h1;

		// to OkLCh
		l1 = l2;
		c1 = std::sqrt(c2 * c2 + h2 * h2);
		h1 = Maths::UnsignedMod(std::atan2(h2, c2), Maths::Tau);

		return OkLCh(l1, c1, h1);
	}
}

sRGB OkLCh::OkLChtosRGB(const OkLCh& oklch) {
	// to reduce floating point error
	if (oklch.GetL() == 1.) return sRGB(1., 1., 1.);
	if (oklch.GetL() == 0.) return sRGB(0., 0., 0.);

	const double scalar = 387916. / 30017.;
	const double limit = 285. / 93752.;

	if (oklch.GetC() == 0.) {
		// if graycale - can skip some conversions

		// can skip "to OkLab" converison
		double v = oklch.GetL();

		// to Linear LMS - can skip "to LMS" conversion
		v = v * v * v;

		// to sRGB - can skip "to Linear RGB" conversion
		v = v <= limit ? scalar * v : (Maths::NRoot(v, 2.4) * 1.055) - 0.055;

		return sRGB(v, v, v);
	} else {
		double r1 = oklch.GetL();
		double g1 = oklch.GetC();
		double b1 = oklch.GetH();

		// to OkLab
		double r2 = r1;
		double g2 = g1 * std::cos(b1);
		double b2 = g1 * std::sin(b1);

		// to LMS
		r1 = r2 + 0.396337792278 * g2 + 0.215803757471 * b2;
		g1 = r2 - 0.105561342920 * g2 - 0.063854171399 * b2;
		b1 = r2 - 0.089484185764 * g2 - 1.291485517099 * b2;

		// to Linear LMS
		r2 = r1 * r1 * r1;
		g2 = g1 * g1 * g1;
		b2 = b1 * b1 * b1;

		// to Linear RGB
		r1 =  4.076741661667 * r2 - 3.307711590572 * g2 + 0.230969928905 * b2;
		g1 = -1.268438004344 * r2 + 2.609757400792 * g2 - 0.341319396448 * b2;
		b1 = -0.004196086474 * r2 - 0.703418614494 * g2 + 1.707614700968 * b2;

		// to sRGB
		r1 = r1 <= limit ? scalar * r1 : (Maths::NRoot(r1, 2.4) * 1.055) - 0.055;
		g1 = g1 <= limit ? scalar * g1 : (Maths::NRoot(g1, 2.4) * 1.055) - 0.055;
		b1 = b1 <= limit ? scalar * b1 : (Maths::NRoot(b1, 2.4) * 1.055) - 0.055;

		return sRGB(r1, g1, b1);
	}
}

OkLCh OkLCh::OkLabtoOkLCh(const OkLab& oklab) {
	// to reduce floating point error
	if (oklab.GetL() == 1.) return OkLCh(1., 0., 0.);
	if (oklab.GetL() == 0.) return OkLCh(0., 0., 0.);
	if (oklab.GetA() == 0. && oklab.GetB() == 0.) return OkLCh(oklab.GetL(), 0., 0.);

	const double l = oklab.GetL();
	const double c = std::sqrt(oklab.GetA() * oklab.GetA() + oklab.GetB() * oklab.GetB());
	const double h = Maths::UnsignedMod(std::atan2(oklab.GetB(), oklab.GetA()), Maths::Tau);
	return OkLCh(l, c, h);
}

OkLab OkLCh::OkLChtoOkLab(const OkLCh& oklch) {
	// to reduce floating point error
	if (oklch.GetL() == 1.) return OkLab(1., 0., 0.);
	if (oklch.GetL() == 0.) return OkLab(0., 0., 0.);
	if (oklch.GetC() == 0.) return OkLab(oklch.GetL(), 0., 0.);

	const double l = oklch.GetL();
	const double a = oklch.GetC() * std::cos(oklch.GetH());
	const double b = oklch.GetC() * std::sin(oklch.GetH());
	return OkLab(l, a, b);
}

std::string OkLCh::Debug(const bool inDegrees) const {
	if (inDegrees) {
		//return std::to_string(m_a) + ' ' + std::to_string(m_b) + ' ' + std::to_string(m_c * Maths::RadToDeg);

		std::stringstream out;
		out << std::fixed << std::setprecision(4);

		out << m_a << ' ';
		out << m_b << ' ';

		if (m_c * Maths::RadToDeg < 10.) {
			out << "  ";
		} else if (m_c * Maths::RadToDeg < 100.) {
			out << " ";
		}
		out << (m_c * Maths::RadToDeg);
		return out.str();
	} else {
		//return std::to_string(m_a) + ' ' + std::to_string(m_b) + ' ' + std::to_string(m_c);

		std::stringstream out;
		out << std::fixed << std::setprecision(4);

		out << m_a << ' ';
		out << m_b << ' ';
		out << m_c;
		return out.str();
	}
}

OkLCh& OkLCh::operator/=(const OkLCh& other) {
	ColorSpace lhs(other);
	ColorSpace::operator/=(lhs);
	m_c = Maths::UnsignedMod(m_c, Maths::Tau);
	return *this;
}

OkLCh& OkLCh::operator*=(const OkLCh& other) {
	ColorSpace lhs(other);
	ColorSpace::operator*=(lhs);
	m_c = Maths::UnsignedMod(m_c, Maths::Tau);
	return *this;
}

OkLCh& OkLCh::operator+=(const OkLCh& other) {
	ColorSpace lhs(other);
	ColorSpace::operator+=(lhs);
	m_c = Maths::UnsignedMod(m_c, Maths::Tau);
	return *this;
}

OkLCh& OkLCh::operator-=(const OkLCh& other) {
	ColorSpace lhs(other);
	ColorSpace::operator-=(lhs);
	m_c = Maths::UnsignedMod(m_c, Maths::Tau);
	return *this;
}

OkLCh& OkLCh::operator*=(const  double scalar) {
	ColorSpace::operator*=(scalar);
	m_c = Maths::UnsignedMod(m_c, Maths::Tau);
	return *this;
}

void OkLCh::Fallback(const int maxIter) {
	int iter = 0;

	while (iter < maxIter) {
		sRGB rgb = OkLCh::OkLChtosRGB(*this);
		rgb.Clamp();

		OkLCh outLCh = OkLCh::sRGBtoOkLCh(rgb);

		m_b = outLCh.GetC();
		m_c = outLCh.GetH();

		if ((*this).IsInsidesRGB()) break;

		iter++;
	}
}

bool OkLCh::IsInsidesRGB() const {
	sRGB current = OkLCh::OkLChtosRGB(*this);
	return current.IsInside();
}