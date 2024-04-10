#pragma once

#include "ColorSpace.h"
#include "sRGB.h"
#include "../maths/Matrix.h"

class OkLab : public ColorSpace {
public:
  OkLab(const double l = 0, const double a = 0, const double b = 0) : ColorSpace(l, a, b) {};
  OkLab(const OkLab& other) : ColorSpace(other) {};

  /// <summary>
  /// Initialise static conversion matrices
  /// </summary>
  static void Initialise();

  inline double GetL() const { return m_a; };
  inline double GetA() const { return m_b; };
  inline double GetB() const { return m_c; };

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
  static double SqrDist(const OkLab& lab1, const OkLab& lab2);

  static double Dist(const OkLab& lab1, const OkLab& lab2);

  OkLab operator*(const double scalar) const;
  OkLab& operator*=(const double scalar);

  OkLab operator+(const OkLab& other) const;
  OkLab& operator+=(const OkLab& other);

  OkLab operator-(const OkLab& other) const;
  OkLab& operator-=(const OkLab& other);

  /// <summary>
  /// Clamp based on its RGB value
  /// </summary>
  void RGBClamp();

private:
  static Matrix LinearRGBtoXYZ;
  static Matrix XYZtoLinearRGB;

  static Matrix XYZtoLinearLMS;
  static Matrix LinearLMStoXYZ;

  static Matrix LMStoLab;
  static Matrix LabtoLMS;
};