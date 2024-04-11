#include "OkLab.h"

Matrix OkLab::LinearRGBtoXYZ({
    {0.4124564, 0.2126729, 0.0193339},
    {0.3575761, 0.7151522, 0.1191920},
    {0.1804375, 0.0721750, 0.9503041}
  });
Matrix OkLab::XYZtoLinearRGB(OkLab::LinearRGBtoXYZ);

Matrix OkLab::XYZtoLinearLMS({
    { 0.8189330101, 0.0329845436, 0.0482003018 },
    { 0.3618667424, 0.9293118715, 0.2643662691 },
    { -0.1288597137, 0.0361456387, 0.6338517070 }
  });
Matrix OkLab::LinearLMStoXYZ(OkLab::XYZtoLinearLMS);

Matrix OkLab::LMStoLab({
    { 0.2104542553, 1.9779984951, 0.0259040371 },
    { 0.7936177850, -2.4285922050, 0.7827717662 },
    { -0.0040720468, 0.4505937099, -0.8086757660 }
  });
Matrix OkLab::LabtoLMS(OkLab::LMStoLab);

void OkLab::Initialise() {
  OkLab::XYZtoLinearRGB.Invert3x3();
  OkLab::LinearLMStoXYZ.Invert3x3();
  OkLab::LabtoLMS.Invert3x3();
}

OkLab OkLab::sRGBtoOkLab(const sRGB& srgb) {
  Matrix val({ {srgb.GetR(), srgb.GetG(), srgb.GetB()} });

  // to Linear RGB
  val.Pow(2.2);

  // to CIE XYZ
  val = OkLab::LinearRGBtoXYZ * val;

  // to Linear LMS
  val = OkLab::XYZtoLinearLMS * val;

  // to LMS
  val.Cbrt();

  // to OkLab
  val = OkLab::LMStoLab * val;

  return OkLab(val.GetValue(0, 0), val.GetValue(0, 1), val.GetValue(0, 2));
}

sRGB OkLab::OkLabtosRGB(const OkLab& lab) {
  Matrix val({ {lab.GetL(), lab.GetA(), lab.GetB()} });

  // to LMS
  val = OkLab::LabtoLMS * val;

  // to Linear LMS
  val.Pow(3.);

  // to CIE XYZ
  val = OkLab::LinearLMStoXYZ * val;

  // to Linear RGB
  val = OkLab::XYZtoLinearRGB * val;

  // to sRGB
  //val.Pow(1. / 2.2);
  val.NRoot(2.2);

  // clamping values

  double r = val.GetValue(0, 0) > 1. ? 1. : val.GetValue(0, 0);
  double g = val.GetValue(0, 1) > 1. ? 1. : val.GetValue(0, 1);
  double b = val.GetValue(0, 2) > 1. ? 1. : val.GetValue(0, 2);

  r = r < 0 ? 0 : r;
  g = g < 0 ? 0 : g;
  b = b < 0 ? 0 : b;

  return sRGB(r, g, b);
}

double OkLab::SqrDist(const OkLab& lab1, const OkLab& lab2) {
  const double l = lab1.m_a - lab2.m_a;
  const double a = lab1.m_b - lab2.m_b;
  const double b = lab1.m_c - lab2.m_c;
  return (l * l) + (a * a) + (b * b);
}

double OkLab::Dist(const OkLab& lab1, const OkLab& lab2) {
  return std::sqrt(OkLab::SqrDist(lab1, lab2));
}

OkLab OkLab::operator*(const double scalar) const {
  OkLab out(*this);
  out *= scalar;
  return out;
}

OkLab& OkLab::operator*=(const double scalar) {
  m_a *= scalar;
  m_b *= scalar;
  m_c *= scalar;
  return *this;
}

OkLab OkLab::operator+(const OkLab& other) const {
  OkLab out(*this);
  out += other;
  return out;
}

OkLab& OkLab::operator+=(const OkLab& other) {
  m_a += other.m_a;
  m_b += other.m_b;
  m_c += other.m_c;
  return *this;
}

OkLab OkLab::operator-(const OkLab& other) const {
  OkLab out(*this);
  out -= other;
  return out;
}

OkLab& OkLab::operator-=(const OkLab& other) {
  m_a -= other.m_a;
  m_b -= other.m_b;
  m_c -= other.m_c;
  return *this;
}

void OkLab::RGBClamp() {
  OkLab lab(m_a, m_b, m_c);
  sRGB rgb = OkLab::OkLabtosRGB(lab);
  rgb.Clamp();

  lab = OkLab::sRGBtoOkLab(rgb);

  m_a = lab.m_a;
  m_b = lab.m_b;
  m_c = lab.m_c;
}