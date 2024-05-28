#pragma once

#include <string>

class ColorSpace {
public:
  ColorSpace(const double a = 0., const double b = 0., const double c = 0.) {
    m_a = a;
    m_b = b;
    m_c = c;
  }
  ColorSpace(const ColorSpace& other) {
    m_a = other.m_a;
    m_b = other.m_b;
    m_c = other.m_c;
  }
  virtual ~ColorSpace() {};

  virtual ColorSpace& operator=(const ColorSpace& other) {
    if (this == &other) return *this;
    m_a = other.m_a;
    m_b = other.m_b;
    m_c = other.m_c;
    return *this;
  }

  virtual std::string Debug() const {
    return std::to_string(m_a) + ' ' + std::to_string(m_b) + ' ' + std::to_string(m_c) + '\n';
  }

  ColorSpace& operator/=(const ColorSpace& other);
  ColorSpace& operator*=(const ColorSpace& other);
  ColorSpace& operator+=(const ColorSpace& other);
  ColorSpace& operator-=(const ColorSpace& other);
  ColorSpace& operator*=(const double scalar);

protected:
  double m_a, m_b, m_c;
};

inline ColorSpace& ColorSpace::operator/=(const ColorSpace& other) {
  m_a /= other.m_a;
  m_b /= other.m_b;
  m_c /= other.m_c;
  return *this;
}

inline ColorSpace& ColorSpace::operator*=(const ColorSpace& other) {
  m_a *= other.m_a;
  m_b *= other.m_b;
  m_c *= other.m_c;
  return *this;
}

inline ColorSpace& ColorSpace::operator+=(const ColorSpace& other) {
  m_a += other.m_a;
  m_b += other.m_b;
  m_c += other.m_c;
  return *this;
}

inline ColorSpace& ColorSpace::operator-=(const ColorSpace& other) {
  m_a -= other.m_a;
  m_b -= other.m_b;
  m_c -= other.m_c;
  return *this;
}

inline ColorSpace& ColorSpace::operator*=(const double scalar) {
  m_a *= scalar;
  m_b *= scalar;
  m_c *= scalar;
  return *this;
}