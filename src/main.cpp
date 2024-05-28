// main.cpp : Defines the entry point for the application.
//

#include "main.h"

const double Maths::Pi = 3.1415926535;
const double Maths::Tau = 6.283185307;
const double Maths::RadToDeg = 180. / Maths::Pi;
const double Maths::DegToRad = Maths::Pi / 180.;

int main(int argc, char* argv[]) {
  /*std::ifstream f("settings.json");

  if (f) {
    
  }
  else {
    Log::StartLine();
    Log::Write("Failed to read: settings.json");
    Log::EndLine();
  }

  Log::Save();*/

  //std::cout << "\nPress enter to exit...\n";
  //std::cin.ignore();

  GeneratePalette("data/minecraft_wool");

  Log::Save();
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

bool GeneratePalette(const std::string& baseFile) {
  const std::string baseFileLoc = baseFile + "_b.palette";
  const std::string flatFileLoc = baseFile + "_f.palette";
  const std::string stairFileLoc = baseFile + "_sc.palette";

  std::fstream bFile(baseFileLoc);

  if (bFile) {
    Log::StartLine();
    Log::Write(baseFileLoc);
    Log::Write(" opened");
    Log::EndLine();

    std::fstream fFile;
    fFile.open(flatFileLoc, std::ios_base::out);

    std::fstream scFile;
    scFile.open(stairFileLoc, std::ios_base::out);

    std::string line;
    while (std::getline(bFile, line)) {
      const sRGB high = sRGB::HexTosRGB(line);
      const sRGB low = high * 0.71;
      const sRGB mid = high * 0.86;

      fFile << mid.sRGBtoHex() + "\n";

      scFile << low.sRGBtoHex() + "\n" + mid.sRGBtoHex() + "\n" + high.sRGBtoHex() + "\n";
    }

    fFile.close();
    scFile.close();
  }
  else {
    Log::StartLine();
    Log::Write(baseFileLoc);
    Log::Write(" not found");
    Log::EndLine();
  }
  bFile.close();

  return false;
}
