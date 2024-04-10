#pragma once

#include <string>

#include "ColorSpace.h"

class sRGB : public ColorSpace {
public:
    sRGB(const double r = 0, const double g = 0, const double b = 0) : ColorSpace(r, g, b) {};
    sRGB(const sRGB& other) : ColorSpace(other) {};

    static sRGB HexTosRGB(const std::string hex);

    inline double GetR() const { return m_a; }
    inline double GetG() const { return m_b; }
    inline double GetB() const { return m_c; }

    void Clamp();

private:

};