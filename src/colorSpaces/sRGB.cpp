#include <sstream>
#include <iomanip>

#include "sRGB.h"

sRGB sRGB::HexTosRGB(const std::string hex) {
  const unsigned int hexInt = std::stoi(hex, 0, 16);

  const unsigned int rMask = 0xFF0000;
  const unsigned int gMask = 0x00FF00;
  const unsigned int bMask = 0x0000FF;

  return sRGB(double((hexInt & rMask) >> 16) / 255., double((hexInt & gMask) >> 8) / 255., double(hexInt & bMask) / 255.);
}

std::string sRGB::GetHex() const {
  std::stringstream out;

  uint8_t r = GetRUInt();
  uint8_t g = GetGUInt();
  uint8_t b = GetBUInt();
  unsigned int hex_i = (r << 16) | (g << 8) | b;

  out << std::setfill('0') << std::uppercase << std::setw(6) << std::hex << hex_i;

  return out.str();
}

void sRGB::Clamp() {
  m_a = m_a > 1 ? 1 : m_a;
  m_b = m_b > 1 ? 1 : m_b;
  m_c = m_c > 1 ? 1 : m_c;

  m_a = m_a < 0 ? 0 : m_a;
  m_b = m_b < 0 ? 0 : m_b;
  m_c = m_c < 0 ? 0 : m_c;
}

sRGB sRGB::operator*(const double scalar) const {
  sRGB out(m_a, m_b, m_c);
  out *= scalar;
  return out;
}

sRGB& sRGB::operator*=(const double scalar) {
  m_a *= scalar;
  m_b *= scalar;
  m_c *= scalar;
  return *this;
}

sRGB sRGB::operator+(const sRGB& other) const {
  sRGB out(m_a, m_b, m_c);
  out += other;
  return out;
}

sRGB& sRGB::operator+=(const sRGB& other) {
  m_a += other.m_a;
  m_b += other.m_b;
  m_c += other.m_c;
  return *this;
}

sRGB sRGB::operator-(const sRGB& other) const {
  sRGB out(m_a, m_b, m_c);
  out -= other;
  return out;
}

sRGB& sRGB::operator-=(const sRGB& other) {
  m_a -= other.m_a;
  m_b -= other.m_b;
  m_c -= other.m_c;
  return *this;
}