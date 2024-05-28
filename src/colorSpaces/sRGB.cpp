#include <sstream>
#include <iomanip>

#include "sRGB.h"

sRGB sRGB::HexTosRGB(const std::string hex) {
  const unsigned int hexInt = std::stoi(hex, 0, 16);

  const unsigned int rMask = 0xFF0000;
  const unsigned int gMask = 0x00FF00;
  const unsigned int bMask = 0x0000FF;

  return sRGB(long double((hexInt & rMask) >> 16) / 255., long double((hexInt & gMask) >> 8) / 255., long double(hexInt & bMask) / 255.);
}

uint8_t sRGB::GetRUInt() const {
  int o = (int)std::round(m_a * 255.);
  o = o > 255 ? 255 : o;
  return uint8_t(o < 0 ? 0 : o);
}

uint8_t sRGB::GetGUInt() const {
  int o = (int)std::round(m_b * 255.);
  o = o > 255 ? 255 : o;
  return uint8_t(o < 0 ? 0 : o);
}

uint8_t sRGB::GetBUInt() const {
  int o = (int)std::round(m_c * 255.);
  o = o > 255 ? 255 : o;
  return uint8_t(o < 0 ? 0 : o);
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

bool sRGB::IsInside() const {
  return m_a >= 0.L && m_b >= 0.L && m_c >= 0.L &&
    m_a <= 1.L && m_b <= 1.L && m_c <= 1.L;
}

sRGB sRGB::operator*(const long double scalar) const {
  sRGB out(m_a, m_b, m_c);
  out *= scalar;
  return out;
}

sRGB& sRGB::operator*=(const long double scalar) {
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