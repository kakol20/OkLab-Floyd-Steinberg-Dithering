#include "../maths/Maths.hpp"
#include "OkLCh.h"
#include <cmath>

OkLCh OkLCh::OkLabtoOkLCh(const OkLab& lab) {
  const long double l = lab.GetL();
  const long double c = std::sqrt(lab.GetA() * lab.GetA() + lab.GetB() * lab.GetB());
  const long double h = Maths::UnsignedMod(std::atan2(lab.GetB(), lab.GetA()), Maths::Tau);

  return OkLCh(l, c, h);
}

OkLab OkLCh::OkLChtoOkLab(const OkLCh& lch) {
  const long double l = lch.GetL();
  const long double a = lch.GetC() * std::cos(lch.GetH());
  const long double b = lch.GetC() * std::sin(lch.GetH());
  return OkLab(l, a, b);
}

void OkLCh::Fallback(const long double change) {
  m_a = std::min(std::max(m_a, 0.L), 1.L);
  m_b = m_a == 0. || m_a == 1. ? 0. : m_b;

  sRGB current = OkLab::OkLabtosRGB(OkLCh::OkLChtoOkLab(*this));
  while (!current.IsInside()) {
    m_b -= change;
    m_b = std::max(m_b, 0.L);

    if (m_b == 0.L) break;
    current = OkLab::OkLabtosRGB(OkLCh::OkLChtoOkLab(*this));
  }
}
