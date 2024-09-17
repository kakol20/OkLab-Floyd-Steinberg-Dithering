﻿// main.cpp : Defines the entry point for the application.
//

#include "main.h"

const double Maths::Pi = 3.1415926535;
const double Maths::Tau = 6.283185307;
const double Maths::RadToDeg = 180. / Maths::Pi;
const double Maths::DegToRad = Maths::Pi / 180.;

int main(int argc, char* argv[]) {
  //GeneratePalette("data/minecraft_map");

  

  Log::Save();

  //std::cout << "\nPress enter to exit...\n";
  //std::cin.ignore();
  return 0;
}

sRGB ClosestPaletteColorRGB(const std::vector<sRGB>& palette, const sRGB& col, const bool lightMode) {
  const OkLab col_lab = OkLab::sRGBtoOkLab(col);

  size_t closest_index = 0;
  double closestDist = OkLab::Distance(col_lab, OkLab::sRGBtoOkLab(palette[closest_index]), lightMode);

  for (size_t i = 1; i < palette.size(); i++) {
    const OkLab currLab = OkLab::sRGBtoOkLab(palette[i]);
    const double currDist = OkLab::Distance(col_lab, currLab, lightMode);

    if (currDist < closestDist) {
      closest_index = i;
      closestDist = currDist;
    }
  }

  return palette[closest_index];
}

OkLab ClosestPaletteColorLab(const std::vector<sRGB>& palette, const OkLab& col, const bool lightMode) {
  size_t closest_index = 0;
  double closestDist = OkLab::Distance(col, OkLab::sRGBtoOkLab(palette[closest_index]), lightMode);

  for (size_t i = 1; i < palette.size(); i++) {
    const OkLab currLab = OkLab::sRGBtoOkLab(palette[i]);
    const double currDist = OkLab::Distance(col, currLab, lightMode);

    if (currDist < closestDist) {
      closest_index = i;
      closestDist = currDist;
    }
  }

  return OkLab::sRGBtoOkLab(palette[closest_index]);
}

bool GetPalette(const std::string& loc, std::vector<sRGB>& out) {
  std::fstream p(loc);
  if (p) {
    /*Log::StartLine();
    Log::Write("----- COLOURS -----");*/
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

sRGB GetRGBFromImage(const Image& img, const int x, const int y, const bool grayscale) {
  const size_t index = img.GetIndex(x, y);

  if (img.GetChannels() == 1 || img.GetChannels() == 2) {
    const double v = sRGB::UInt8ToDouble(img.GetData(index));
    return sRGB(v, v, v);
  }
  else {
    const double r = sRGB::UInt8ToDouble(img.GetData(index + 0));
    const double g = sRGB::UInt8ToDouble(img.GetData(index + 1));
    const double b = sRGB::UInt8ToDouble(img.GetData(index + 2));

    if (grayscale) {
      const OkLab lab = OkLab::sRGBtoOkLab(sRGB(r, g, b)) * OkLab(1., 0., 0.);
      return OkLab::OkLabtosRGB(lab);
    }
    else {
      return sRGB(r, g, b);
    }
  }
}

void SetDataFromRGB(Image& img, const int x, const int y, const sRGB& srgb) {
  const size_t index = img.GetIndex(x, y);
  if (img.GetChannels() == 1 || img.GetChannels() == 2) {
    const OkLab lab = OkLab::sRGBtoOkLab(srgb) * OkLab(1., 0., 0.);
    const sRGB gray = OkLab::OkLabtosRGB(lab);

    const uint8_t v = gray.GetRUInt();

    img.SetData(index, v);
  }
  else {
    const uint8_t r = srgb.GetRUInt();
    const uint8_t g = srgb.GetGUInt();
    const uint8_t b = srgb.GetBUInt();

    img.SetData(index + 0, r);
    img.SetData(index + 1, g);
    img.SetData(index + 2, b);
  }
}

size_t GetIndex(const int x, const int y, const int width, const int channels) {
  // size_t((x + y * m_w) * m_channels)
  return size_t((x + y * width) * channels);
}
