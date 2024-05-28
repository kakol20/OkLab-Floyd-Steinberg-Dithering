#pragma once

#include <cmath>

class Maths {
public:
  Maths() {};
  ~Maths() {};

  static long double NRoot(const double v, const double n) {
    long double out = v;
    const long double exp = 1. / n;
    if (std::fmod(n, 1.) == 0.) {
      out = std::pow(out, exp);
    }
    else {
      long double absroot = std::pow(std::abs(out), exp);
      if (out < 0.) absroot *= -1.;
      out = absroot;
    }
    return out;
  }

  static long double UnsignedMod(const long double numer, const long double denom) {
    return std::fmod(std::fmod(numer, denom) + denom, denom);
  }

  static const long double Pi;
  static const long double Tau;
  static const long double RadToDeg;
  static const long double DegToRad;
};