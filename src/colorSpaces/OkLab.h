#pragma once

#include "ColorSpace.hpp"
#include "sRGB.hpp"

class OkLab : public ColorSpace {
public:
  OkLab(const double l = 0., const double a = 0., const double b = 0.) : ColorSpace(l, a, b) {};
  OkLab(const OkLab& other) : ColorSpace(other) {};

  double GetL() const { return m_a; };
  double GetA() const { return m_b; };
  double GetB() const { return m_c; };

  static OkLab sRGBtoOkLab(const sRGB& srgb);
  static sRGB OkLabtosRGB(const OkLab& oklab);

  OkLab& operator/=(const OkLab& other);
  OkLab& operator*=(const OkLab& other);
  OkLab& operator+=(const OkLab& other);
  OkLab& operator-=(const OkLab& other);
  OkLab& operator*=(const double scalar);

  OkLab operator/(const OkLab& other) const { OkLab out(*this); out /= other; return out; };
  OkLab operator*(const OkLab& other) const { OkLab out(*this); out *= other; return out; };
  OkLab operator+(const OkLab& other) const { OkLab out(*this); out += other; return out; };
  OkLab operator-(const OkLab& other) const { OkLab out(*this); out -= other; return out; };
  OkLab operator*(const double scalar) const { OkLab out(*this); out *= scalar; return out; };

  bool IsInsidesRGB() const;

  static double Distance(const OkLab& lab1, const OkLab& lab2, const bool lightMode = false);
};