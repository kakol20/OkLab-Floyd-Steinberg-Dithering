#include "OkLab.h"
#include "../maths/Maths.hpp"

//#define USE_MATRIX

Matrix OkLab::LinearRGBtoXYZ({
  0.4124564, 0.3575761, 0.1804375,
  0.2126729, 0.7151522, 0.0721750,
  0.0193339, 0.1191920, 0.9503041 }, 3, 3);

Matrix OkLab::XYZtoLinearRGB(OkLab::LinearRGBtoXYZ);

Matrix OkLab::XYZtoLinearLMS({
  0.8189330101, 0.3618667424, -0.1288597137,
  0.0329845436, 0.9293118715, 0.0361456387,
  0.0482003018, 0.2643662691, 0.6338517070 }, 3, 3);

Matrix OkLab::LinearLMStoXYZ(OkLab::XYZtoLinearLMS);

Matrix OkLab::LMStoLab({
  0.2104542553, 0.7936177850, -0.0040720468,
  1.9779984951, -2.4285922050, 0.4505937099,
  0.0259040371, 0.7827717662, -0.8086757660 }, 3, 3);
Matrix OkLab::LabtoLMS(OkLab::LMStoLab);

Matrix OkLab::LinearRGBtoLinearLMS(OkLab::XYZtoLinearLMS);
Matrix OkLab::LinearLMStoLinearRGB(OkLab::LinearRGBtoLinearLMS);

void OkLab::Initialise() {
  OkLab::LinearRGBtoLinearLMS = OkLab::XYZtoLinearLMS * OkLab::LinearRGBtoXYZ;
  OkLab::LinearLMStoLinearRGB = OkLab::LinearRGBtoLinearLMS;
  OkLab::LinearLMStoLinearRGB.Invert3x3();

  OkLab::XYZtoLinearRGB.Invert3x3();
  OkLab::LinearLMStoXYZ.Invert3x3();
  OkLab::LabtoLMS.Invert3x3();
}

#ifdef USE_MATRIX
OkLab OkLab::sRGBtoOkLab(const sRGB& srgb) {
  Matrix val({ srgb.GetR(), srgb.GetG(), srgb.GetB() }, 1, 3);

  // to Linear RGB
  val.Pow(2.224874);

  // to CIE XYZ
  //val = OkLab::LinearRGBtoXYZ * val;

  // to Linear LMS
  //val = OkLab::XYZtoLinearLMS * val;

  // to Linear LMS
  val = OkLab::LinearRGBtoLinearLMS * val;

  // to LMS
  val.Cbrt();

  // to OkLab
  val = OkLab::LMStoLab * val;

  return OkLab(val(0, 0), val(0, 1), val(0, 2));
}

sRGB OkLab::OkLabtosRGB(const OkLab& lab) {
  Matrix val({ lab.GetL(), lab.GetA(), lab.GetB() }, 1, 3);

  // to LMS
  val = OkLab::LabtoLMS * val;

  // to Linear LMS
  val.Pow(3.);

  // to CIE XYZ
  //val = OkLab::LinearLMStoXYZ * val;

  // to Linear RGB
  //val = OkLab::XYZtoLinearRGB * val;

  // to Linear RGB
  val = OkLab::LinearLMStoLinearRGB * val;

  // to sRGB
  //val.Pow(1. / 2.2);
  val.NRoot(2.224874);

  // clamping values

  // double r = val.GetValue(0, 0) > 1. ? 1. : val.GetValue(0, 0);
  // double g = val.GetValue(0, 1) > 1. ? 1. : val.GetValue(0, 1);
  // double b = val.GetValue(0, 2) > 1. ? 1. : val.GetValue(0, 2);

  // r = r < 0 ? 0 : r;
  // g = g < 0 ? 0 : g;
  // b = b < 0 ? 0 : b;

  double r = val(0, 0);
  double g = val(0, 1);
  double b = val(0, 2);

  return sRGB(r, g, b);
}
#else
OkLab OkLab::sRGBtoOkLab(const sRGB& srgb) {
  //Matrix val({ srgb.GetR(), srgb.GetG(), srgb.GetB() }, 1, 3);
  double r1 = srgb.GetR();
  double g1 = srgb.GetG();
  double b1 = srgb.GetB();

  // to Linear RGB
  //val.Pow(2.224874);
  r1 = std::pow(r1, 2.224874);
  g1 = std::pow(g1, 2.224874);
  b1 = std::pow(b1, 2.224874);

  // to Linear LMS
  //val = OkLab::LinearRGBtoLinearLMS * val;
  double r2 = 0.412242 * r1 + 0.536262 * g1 + 0.051428 * b1;
  double g2 = 0.211943 * r1 + 0.680702 * g1 + 0.107374 * b1;
  double b2 = 0.088359 * r1 + 0.281847 * g1 + 0.630130 * b1;

  // to LMS
  //val.Cbrt()
  r1 = std::cbrt(r2);
  g1 = std::cbrt(g2);
  b1 = std::cbrt(b2);

  // to OkLab
  //val = OkLab::LMStoLab * val;
  r2 = 0.210454 * r1 + 0.793618 * g1 - 0.004072 * b1;
  g2 = 1.977998 * r1 - 2.428592 * g1 + 0.450594 * b1;
  b2 = 0.025904 * r1 + 0.782772 * g1 - 0.808676 * b1;

  return OkLab(r2, g2, b2);
}
sRGB OkLab::OkLabtosRGB(const OkLab& lab) {
  //Matrix val({ lab.GetL(), lab.GetA(), lab.GetB() }, 1, 3);
  double l1 = lab.GetL();
  double a1 = lab.GetA();
  double b1 = lab.GetB();

  // to LMS
  //val = OkLab::LabtoLMS * val;
  double l2 = l1 + 0.396338 * a1 + 0.215804 * b1;
  double a2 = l1 - 0.105561 * a1 - 0.063854 * b1;
  double b2 = l1 - 0.089484 * a1 - 1.291486 * b1;

  // to Linear LMS
  //val.Pow(3.);
  l1 = l2 * l2 * l2;
  a1 = a2 * a2 * a2;
  b1 = b2 * b2 * b2;

  // to Linear RGB
  //val = OkLab::LinearLMStoLinearRGB * val;
  l2 = 4.076539 * l1 - 3.307097 * a1 + 0.230822 * b1;
  a2 = -1.268606 * l1 + 2.609748 * a1 - 0.341164 * b1;
  b2 = -0.004198 * l1 - 0.703568 * a1 + 1.707206 * b1;

  // to sRGB
  //val.Pow(1. / 2.2);
  //val.NRoot(2.224874);
  l1 = Maths::NRoot(l2, 2.224874);
  a1 = Maths::NRoot(a2, 2.224874);
  b1 = Maths::NRoot(b2, 2.224874);

  return sRGB(l1, a1, b1);
}
#endif // USE_MATRIX

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

/// <summary>
/// Checks if colour is grayscale
/// </summary>
/// <param name="threshold">Uses OkLab colour space so small number may be required such as 0.01</param>
/// <returns></returns>
bool OkLab::IsGrayscale(const double threshold) const {
  const double t = threshold * threshold;
  const OkLab gray(m_a, 0., 0.);
  return OkLab::SqrDist(gray, *this) <= t;
}

std::string OkLab::DebugsRGBtoOkLabMats() {
  std::string out = "Linear RGB to Linear LMS\n";
  out += OkLab::LinearRGBtoLinearLMS.Debug(2);
  out += "LMS to OkLab\n";
  out += OkLab::LMStoLab.Debug(2);

  return out;
}

std::string OkLab::DebugOkLabtosRGBMats() {
  std::string out = "OkLab to LMS\n";
  out += OkLab::LabtoLMS.Debug(2);
  out += "Linear LMS to Linear RGB\n";
  out += OkLab::LinearLMStoLinearRGB.Debug(2);
  return out;
}
