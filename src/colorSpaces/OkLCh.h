#pragma once

#include "ColorSpace.h"
#include "OkLab.h"

class OkLCh : public ColorSpace {
public:
  OkLCh(const long double l = 0, const long double c = 0, const long double h = 0) : ColorSpace(l, c, h) {};
  OkLCh(const OkLCh& other) : ColorSpace(other) {};

  inline long double GetL() const { return m_a; };
  inline long double GetC() const { return m_b; };
  inline long double GetH() const { return m_c; };

  /// <summary>
  /// Convert OkLab to OkLCh
  /// </summary>
  /// <param name="lab"></param>
  /// <returns></returns>
  static OkLCh OkLabtoOkLCh(const OkLab& lab);

  /// <summary>
  /// Convert OkLCh to OkLab
  /// </summary>
  /// <param name="lch"></param>
  /// <returns></returns>
  static OkLab OkLChtoOkLab(const OkLCh& lch);

  void Fallback(const long double change = 0.001);
};