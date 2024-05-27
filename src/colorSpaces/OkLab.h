#pragma once

#include "ColorSpace.h"
#include "sRGB.h"
#include "../maths/Matrix.h"

class OkLab : public ColorSpace {
public:
  OkLab(const long double l = 0, const long double a = 0, const long double b = 0) : ColorSpace(l, a, b) {};
  OkLab(const OkLab& other) : ColorSpace(other) {};

  /// <summary>
  /// Initialise static conversion matrices
  /// </summary>
  static void Initialise();

  inline long double GetL() const { return m_a; };
  inline long double GetA() const { return m_b; };
  inline long double GetB() const { return m_c; };

  /// <summary>
  /// Convert sRGB to OkLab
  /// </summary>
  /// <param name="srgb"></param>
  /// <returns></returns>
  static OkLab sRGBtoOkLab(const sRGB& srgb);

  /// <summary>
  /// Convert OkLab to sRGB
  /// </summary>
  /// <param name="lab"></param>
  /// <returns></returns>
  static sRGB OkLabtosRGB(const OkLab& lab);

  /// <summary>
  /// Find the square distance between two OkLabs
  /// </summary>
  /// <param name="lab1"></param>
  /// <param name="lab2"></param>
  /// <returns></returns>
  static long double SqrDist(const OkLab& lab1, const OkLab& lab2);

  static long double Dist(const OkLab& lab1, const OkLab& lab2);

  OkLab operator*(const long double scalar) const;
  OkLab& operator*=(const long double scalar);

  OkLab operator+(const OkLab& other) const;
  OkLab& operator+=(const OkLab& other);

  OkLab operator-(const OkLab& other) const;
  OkLab& operator-=(const OkLab& other);

  /// <summary>
  /// Clamp based on its RGB value
  /// </summary>
  void RGBClamp();

  bool IsGrayscale(const long double threshold) const;

  inline void ConvertToGrayscale() { m_b = 0., m_c = 0.; };

  static std::string DebugsRGBtoOkLabMats();
  static std::string DebugOkLabtosRGBMats();

private:
  static Matrix LinearRGBtoXYZ;
  static Matrix XYZtoLinearRGB;

  static Matrix XYZtoLinearLMS;
  static Matrix LinearLMStoXYZ;

  static Matrix LMStoLab;
  static Matrix LabtoLMS;

  static Matrix LinearRGBtoLinearLMS;
  static Matrix LinearLMStoLinearRGB;
};