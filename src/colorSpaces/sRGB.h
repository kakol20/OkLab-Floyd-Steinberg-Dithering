#pragma once

#include <string>
#include <cmath>

#include "ColorSpace.h"

class sRGB : public ColorSpace {
public:
  sRGB(const long double r = 0, const long double g = 0, const long double b = 0) : ColorSpace(r, g, b) {};
  sRGB(const sRGB& other) : ColorSpace(other) {};

  static sRGB HexTosRGB(const std::string hex);

  inline long double GetR() const { return m_a; }
  inline long double GetG() const { return m_b; }
  inline long double GetB() const { return m_c; }

  //inline uint8_t GetRUInt() const { return uint8_t(std::round(m_a * 255.)); };
  //inline uint8_t GetGUInt() const { return uint8_t(std::round(m_b * 255.)); };
  //inline uint8_t GetBUInt() const { return uint8_t(std::round(m_c * 255.)); };

  uint8_t GetRUInt() const;
  uint8_t GetGUInt() const;
  uint8_t GetBUInt() const;

  std::string GetHex() const;

  static long double UInt8toDouble(const uint8_t v) { return long double(v) / 255.; };

  void Clamp();

  bool IsInside() const;

  sRGB operator*(const long double scalar) const;
  sRGB& operator*=(const long double scalar);

  sRGB operator+(const sRGB& other) const;
  sRGB& operator+=(const sRGB& other);

  sRGB operator-(const sRGB& other) const;
  sRGB& operator-=(const sRGB& other);

private:
};