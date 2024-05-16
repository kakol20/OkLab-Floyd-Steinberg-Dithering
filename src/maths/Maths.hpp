#pragma once

#include <cmath>

namespace Maths {
  double NRoot(const double v, const double n) {
    double out = v;
    const double exp = 1. / n;
    if (std::fmod(n, 1.) == 0.) {
      out = std::pow(out, exp);
    }
    else {
      double absroot = std::pow(std::abs(out), exp);
      if (out < 0.) absroot *= -1.;
      out = absroot;
    }
    return out;
  }
}