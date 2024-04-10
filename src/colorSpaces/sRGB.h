#pragma once

#include <string>

#include "ColorSpace.h"

class sRGB : public ColorSpace {
public:
    sRGB(const double r, const double g, const double b) : ColorSpace(r, g, b) {};
    sRGB(const sRGB& other) : ColorSpace(other) {};

    static sRGB HexTosRGB(const std::string hex);

    inline double GetR() const { return m_a; }
    inline double GetG() const { return m_b; }
    inline double GetB() const { return m_c; }

private:

};