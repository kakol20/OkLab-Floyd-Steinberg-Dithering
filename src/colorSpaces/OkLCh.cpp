#include "../maths/Maths.hpp"
#include "OkLCh.h"

OkLCh OkLCh::sRGBtoOkLCh(const sRGB& srgb) {
  double l1 = srgb.GetR();
  double c1 = srgb.GetG();
  double h1 = srgb.GetB();

  // to Linear RGB
  l1 = l1 <= 0.04045 ? l1 / 12.92 : std::pow((l1 + 0.055) / 1.055, 2.4);
  c1 = c1 <= 0.04045 ? c1 / 12.92 : std::pow((c1 + 0.055) / 1.055, 2.4);
  h1 = h1 <= 0.04045 ? h1 / 12.92 : std::pow((h1 + 0.055) / 1.055, 2.4);

  // to Linear LMS
  double l2 = 0.41224204988807 * l1 + 0.53626162185168 * c1 + 0.05142804288870 * h1;
  double c2 = 0.21194297298929 * l1 + 0.68070218481804 * c1 + 0.10737408156507 * h1;
  double h2 = 0.08835888958899 * l1 + 0.28184744754987 * c1 + 0.63012965338243 * h1;

  // to LMS
  //val.Cbrt()
  l1 = std::cbrt(l2);
  c1 = std::cbrt(c2);
  h1 = std::cbrt(h2);

  // to OkLab
  l2 = 0.21045425666795 * l1 + 0.79361779015852 * c1 - 0.00407204682647 * h1;
  c2 = 1.97799849510000 * l1 - 2.42859220500000 * c1 + 0.45059370990000 * h1;
  h2 = 0.02590402925006 * l1 + 0.78277173659806 * c1 - 0.80867576584811 * h1;

  // to OkLCh
  l1 = l2;
  c1 = std::sqrt(c2 * c2 + h2 * h2);
  h1 = Maths::UnsignedMod(std::atan2(h2, c2), Maths::Tau);

  return OkLCh(l1, c1, h1);
}

sRGB OkLCh::OkLChtosRGB(const OkLCh& oklch) {
  double r1 = oklch.GetL();
  double g1 = oklch.GetC();
  double b1 = oklch.GetH();

  // to OkLab
  double r2 = r1;
  double g2 = g1 * std::cos(b1);
  double b2 = g1 * std::sin(b1);

  // to LMS
  r1 = r2 + 0.39633779217377 * g2 + 0.21580375806076 * b2;
  g1 = r2 - 0.10556134232366 * g2 - 0.06385417477171 * b2;
  b1 = r2 - 0.08948418209497 * g2 - 1.29148553786409 * b2;

  // to Linear LMS
  r2 = r1 * r1 * r1;
  g2 = g1 * g1 * g1;
  b2 = b1 * b1 * b1;

  // to Linear RGB
  r1 =  4.07653881638861 * r2 - 3.30709682773943 * g2 + 0.23082245163012 * b2;
  g1 = -1.26860625095165 * r2 + 2.60974767679763 * g2 - 0.34116363525495 * b2;
  b1 = -0.00419756377401 * r2 - 0.70356840947339 * g2 + 1.70720561792434 * b2;

  // to sRGB
  r1 = r1 <= 0.00313058 ? 12.92 * r1 : (Maths::NRoot(r1, 2.4) * 1.055) - 0.055;
  g1 = g1 <= 0.00313058 ? 12.92 * g1 : (Maths::NRoot(g1, 2.4) * 1.055) - 0.055;
  b1 = b1 <= 0.00313058 ? 12.92 * b1 : (Maths::NRoot(b1, 2.4) * 1.055) - 0.055;

  return sRGB(r1, g1, b1);
}

OkLCh OkLCh::OkLabtoOkLCh(const OkLab& oklab) {
  const double l = oklab.GetL();
  const double c = std::sqrt(oklab.GetA() * oklab.GetA() + oklab.GetB() * oklab.GetB());
  const double h = Maths::UnsignedMod(std::atan2(oklab.GetB(), oklab.GetA()), Maths::Tau);
  return OkLCh(l, c, h);
}

OkLab OkLCh::OkLChtoOkLab(const OkLCh& oklch) {
  const double l = oklch.GetL();
  const double a = oklch.GetC() * std::cos(oklch.GetH());
  const double b = oklch.GetC() * std::sin(oklch.GetH());
  return OkLab(l, a, b);
}

std::string OkLCh::Debug(const bool inDegrees) const {
  if (inDegrees) {
    return std::to_string(m_a) + ' ' + std::to_string(m_b) + ' ' + std::to_string(m_c * Maths::RadToDeg);
  }
  else {
    return std::to_string(m_a) + ' ' + std::to_string(m_b) + ' ' + std::to_string(m_c);
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