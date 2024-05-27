#pragma once

#include <cmath>

namespace Maths {
  long double NRoot(const long double v, const long double n) {
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
}