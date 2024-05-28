#include "OkLab.h"

#include "../maths/Maths.hpp"

OkLab OkLab::sRGBtoOkLab(const sRGB& srgb) {
  double l1 = srgb.GetR();
  double a1 = srgb.GetG();
  double b1 = srgb.GetB();

  // to Linear RGB
  l1 = l1 <= 0.04045 ? l1 / 12.92 : std::pow((l1 + 0.055) / 1.055, 2.4);
  a1 = a1 <= 0.04045 ? a1 / 12.92 : std::pow((a1 + 0.055) / 1.055, 2.4);
  b1 = b1 <= 0.04045 ? b1 / 12.92 : std::pow((b1 + 0.055) / 1.055, 2.4);

  // to Linear LMS
  double l2 = 0.4122214708 * l1 + 0.5363325363 * a1 + 0.0514459929 * b1;
  double a2 = 0.2119034982 * l1 + 0.6806995451 * a1 + 0.1073969566 * b1;
  double b2 = 0.0883024619 * l1 + 0.2817188376 * a1 + 0.6299787005 * b1;

  // to LMS
  l1 = std::cbrt(l2);
  a1 = std::cbrt(a2);
  b1 = std::cbrt(b2);

  // to OkLab
  l2 = 0.2104542553 * l1 + 0.7936177850 * a1 - 0.0040720468 * b1;
  a2 = 1.9779984951 * l1 - 2.4285922050 * a1 + 0.4505937099 * b1;
  b2 = 0.0259040371 * l1 + 0.7827717662 * a1 - 0.8086757660 * b1;

  return OkLab(l2, a2, b2);
}

sRGB OkLab::OkLabtosRGB(const OkLab& oklab) {
  double r1 = oklab.GetL();
  double g1 = oklab.GetA();
  double b1 = oklab.GetB();

  // to LMS
  double r2 = r1 + 0.3963377774 * g1 + 0.2158037573 * b1;
  double g2 = r1 - 0.1055613458 * g1 - 0.0638541728 * b1;
  double b2 = r1 - 0.0894841775 * g1 - 1.2914855480 * b1;

  // to Linear LMS
  r1 = r2 * r2 * r2;
  g1 = g2 * g2 * g2;
  b1 = b2 * b2 * b2;

  // to Linear RGB
  r2 =  4.0767416621 * r1 - 3.3077115913 * g1 + 0.2309699292 * b1;
  g2 = -1.2684380046 * r1 + 2.6097574011 * g1 - 0.3413193965 * b1;
  b2 = -0.0041960863 * r1 - 0.7034186147 * g1 + 1.7076147010 * b1;

  // to sRGB
  r2 = r2 <= 0.00313058 ? 12.92 * r2 : (Maths::NRoot(r2, 2.4) * 1.055) - 0.055;
  g2 = g2 <= 0.00313058 ? 12.92 * g2 : (Maths::NRoot(g2, 2.4) * 1.055) - 0.055;
  b2 = b2 <= 0.00313058 ? 12.92 * b2 : (Maths::NRoot(b2, 2.4) * 1.055) - 0.055;
  return sRGB(r2, g2, b2);
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
  }
  else {
    const OkLab delta = lab1 - lab2;
    return std::sqrt(delta.GetL() * delta.GetL() + delta.GetA() * delta.GetA() + delta.GetB() * delta.GetB());
  }
  return 0.0;
}
