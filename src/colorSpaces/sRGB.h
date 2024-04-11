#pragma once

#include <string>
#include <cmath>

#include "ColorSpace.h"

class sRGB : public ColorSpace {
public:
  sRGB(const double r = 0, const double g = 0, const double b = 0) : ColorSpace(r, g, b) {};
  sRGB(const sRGB& other) : ColorSpace(other) {};

  static sRGB HexTosRGB(const std::string hex);

  inline double GetR() const { return m_a; }
  inline double GetG() const { return m_b; }
  inline double GetB() const { return m_c; }

  inline uint8_t GetRUInt() const { return uint8_t(std::round(m_a * 255.)); };
  inline uint8_t GetGUInt() const { return uint8_t(std::round(m_b * 255.)); };
  inline uint8_t GetBUInt() const { return uint8_t(std::round(m_c * 255.)); };

  std::string GetHex() const;

  static double UInt8toDouble(const uint8_t v) { return double(v) / 255.; };

  void Clamp();

  sRGB operator*(const double scalar) const;
  sRGB& operator*=(const double scalar);

  sRGB operator+(const sRGB& other) const;
  sRGB& operator+=(const sRGB& other);

  sRGB operator-(const sRGB& other) const;
  sRGB& operator-=(const sRGB& other);

private:
};