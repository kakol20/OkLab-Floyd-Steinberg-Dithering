// main.cpp : Defines the entry point for the application.
//

#include "main.h"

const double Maths::Pi = 3.1415926535;
const double Maths::Tau = 6.283185307;
const double Maths::RadToDeg = 180. / Maths::Pi;
const double Maths::DegToRad = Maths::Pi / 180.;

int main(int argc, char* argv[]) {
  std::ifstream f("settings.json");

  if (f) {
    
  }
  else {
    Log::StartLine();
    Log::Write("Failed to read: settings.json");
    Log::EndLine();
  }

  Log::Save();

  //std::cout << "\nPress enter to exit...\n";
  //std::cin.ignore();
  return 0;
}

sRGB ClosestPaletteColorRGB(const std::vector<sRGB>& palette, const sRGB& col) {
  /*const OkLab col_lab = OkLab::sRGBtoOkLab(col);
  OkLab closest = palette[0];
  long double closestDist = OkLab::SqrDist(col_lab, closest);

  for (size_t i = 1; i < palette.size(); i++) {
    long double dist = OkLab::SqrDist(col_lab, palette[i]);

    if (dist < closestDist) {
      closest = palette[i];
      closestDist = dist;
    }
  }*/

  //return OkLab::OkLabtosRGB(closest);
  return sRGB();
}

OkLab ClosestPaletteColorLAB(const std::vector<OkLab>& palette, const OkLab& lab) {
  /*OkLab closest = palette[0];
  long double closestDist = OkLab::SqrDist(lab, closest);

  for (size_t i = 1; i < palette.size(); i++) {
    long double dist = OkLab::SqrDist(lab, palette[i]);

    if (dist < closestDist) {
      closest = palette[i];
      closestDist = dist;
    }
  }
  return closest;*/
  return OkLab();
}