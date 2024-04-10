#include "sRGB.h"

sRGB sRGB::HexTosRGB(const std::string hex) {
    const unsigned int hexInt = std::stoi(hex, 0, 16);

    const unsigned int rMask = 0xFF0000;
    const unsigned int gMask = 0x00FF00;
    const unsigned int bMask = 0x0000FF;

    return sRGB(double((hexInt & rMask) >> 16) / 255., double((hexInt & gMask) >> 8) / 255., double(hexInt & bMask) / 255.);
}

void sRGB::Clamp() {
    m_a = m_a > 1 ? 1 : m_a;
    m_b = m_b > 1 ? 1 : m_b;
    m_c = m_c > 1 ? 1 : m_c;

    m_a = m_a < 0 ? 0 : m_a;
    m_b = m_b < 0 ? 0 : m_b;
    m_c = m_c < 0 ? 0 : m_c;
}
