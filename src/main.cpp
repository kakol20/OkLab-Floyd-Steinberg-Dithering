// main.cpp : Defines the entry point for the application.
//

#include "main.h"

const double Maths::Pi = 3.1415926535;
const double Maths::Tau = 6.283185307;
const double Maths::RadToDeg = 180. / Maths::Pi;
const double Maths::DegToRad = Maths::Pi / 180.;

int main(int argc, char* argv[]) {
  //std::cout << "\nPress enter to exit...\n";
  //std::cin.ignore();

  //GeneratePalette("data/minecraft_map_gs");

  std::ifstream f("settings.json");
  if (f) {
    json settings = json::parse(f);

    // ----- CHECK IF SETTING EXISTS -----

    bool success = true;

    if (!settings.contains("input_image")) {
      Log::StartLine();
      Log::Write("input setting not found");
      Log::EndLine();
      success = false;
    }

    if (!settings.contains("output_image")) {
      Log::StartLine();
      Log::Write("output_image setting not found");
      Log::EndLine();
      success = false;
    }

    if (!settings.contains("palette")) {
      Log::StartLine();
      Log::Write("palette setting not found");
      Log::EndLine();
      success = false;
    }

    if (!settings.contains("dither")) {
      Log::StartLine();
      Log::Write("dither setting not found");
      Log::EndLine();
      success = false;
    }

    if (!settings.contains("grayscale")) {
      Log::StartLine();
      Log::Write("grayscale setting not found");
      Log::EndLine();
      success = false;
    }

    if (!settings.contains("dist_lightness")) {
      Log::StartLine();
      Log::Write("dist_lightness setting not found");
      Log::EndLine();
      success = false;
    }

    if (success) {
      // ----- READ SETTINGS -----

      Log::StartLine();
      Log::Write("----- SETTINGS -----");
      Log::EndLine();

      const std::string inputLoc = settings["input_image"];
      Log::StartLine();
      Log::Write("Input: " + inputLoc);
      Log::EndLine();

      const std::string outputLoc = settings["output_image"];
      Log::StartLine();
      Log::Write("Output: " + outputLoc);
      Log::EndLine();

      const std::string paletteLoc = settings["palette"];
      Log::StartLine();
      Log::Write("Palette: " + paletteLoc);
      Log::EndLine();

      const bool dither = settings["dither"];
      Log::StartLine();
      Log::Write("Dither: ");
      Log::Write(dither ? "TRUE" : "FALSE");
      Log::EndLine();

      const bool grayscale = settings["grayscale"];
      Log::StartLine();
      Log::Write("Process As Grayscale: ");
      Log::Write(grayscale ? "TRUE" : "FALSE");
      Log::EndLine();

      const bool dist_l = settings["dist_lightness"];
      Log::StartLine();
      Log::Write("Use Lightness As Distance: ");
      Log::Write(dist_l ? "TRUE" : "FALSE");
      Log::EndLine();

      // ----- GET PALETTE -----

      std::vector<sRGB> palette;
      success = success && GetPalette(paletteLoc, palette);

      // ----- GET INPUT IMAGE -----
      Log::StartLine();
      Log::Write("----- INPUT IMAGE -----");
      Log::EndLine();

      Image inputImg;
      success = success && inputImg.Read(inputLoc.c_str());

      if (success) {
      }
    }
  }
  else {
    Log::StartLine();
    Log::Write("Failed to read: settings.json");
    Log::EndLine();
  }

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

bool GetPalette(const std::string& loc, std::vector<sRGB>& out) {
  std::fstream p(loc);
  if (p) {
    Log::StartLine();
    Log::Write("----- COLOURS -----");
    Log::EndLine();

    std::string hex;
    while (std::getline(p, hex)) {
      const sRGB rgb = sRGB::HexTosRGB(hex);
      const OkLab lab = OkLab::sRGBtoOkLab(rgb);
      const OkLCh lch = OkLCh::OkLabtoOkLCh(lab);

      out.push_back(rgb);

      Log::StartLine();
      Log::Write("#");
      Log::Write(hex);
      Log::Write(" --- rgb(");
      Log::Write(rgb.UintDebug());
      Log::Write(") --- oklab(");
      Log::Write(lab.Debug());
      Log::Write(") --- oklch(");
      Log::Write(lch.Debug());
      Log::Write(")");
      Log::EndLine();
    }

    return true;
  }
  else {
    Log::StartLine();
    Log::Write(loc);
    Log::Write(" not found");
    Log::EndLine();

    p.close();
    return false;
  }
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
