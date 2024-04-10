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

private:
    static Matrix LinearRGBtoXYZ;
    static Matrix XYZtoLinearRGB;

    static Matrix XYZtoLinearLMS;
    static Matrix LinearLMStoXYZ;

    static Matrix LMStoLab;
    static Matrix LabtoLMS;
};