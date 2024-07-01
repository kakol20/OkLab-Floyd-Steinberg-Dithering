// main.cpp : Defines the entry point for the application.
//

#include "main.h"

const double Maths::Pi = 3.1415926535;
const double Maths::Tau = 6.283185307;
const double Maths::RadToDeg = 180. / Maths::Pi;
const double Maths::DegToRad = Maths::Pi / 180.;

int main(int argc, char* argv[]) {
  //GeneratePalette("data/minecraft_map");

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

      /*Log::StartLine();
      Log::Write("----- SETTINGS -----");*/
      //Log::EndLine();

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

      //Log::EndLine();

      std::vector<sRGB> palette;
      success = success && GetPalette(paletteLoc, palette);

      // ----- GET INPUT IMAGE -----
      /*Log::StartLine();
      Log::Write("----- INPUT IMAGE -----");
      Log::EndLine();*/

      Log::EndLine();

      Image inputImg;
      success = success && inputImg.Read(inputLoc.c_str());

      if (success) {
        Log::StartLine();
        Log::Write("Width: ");
        Log::Write(std::to_string(inputImg.GetWidth()));
        Log::EndLine();

        Log::StartLine();
        Log::Write("Height: ");
        Log::Write(std::to_string(inputImg.GetHeight()));
        Log::EndLine();

        // ----- MAIN PROCESS -----

        Log::EndLine();

        std::vector<OkLab> pixelsLab;
        pixelsLab.reserve((size_t)(inputImg.GetWidth() * inputImg.GetHeight()));

        // -- Copy Pixels into vector of OkLab --
        Log::StartLine();
        Log::Write("Copying pixels...");

        Log::StartTime();
        for (int y = 0; y < inputImg.GetHeight(); y++) {
          for (int x = 0; x < inputImg.GetWidth(); x++) {
            const sRGB pixelCol = GetRGBFromImage(inputImg, x, y);
            pixelsLab.push_back(OkLab::sRGBtoOkLab(pixelCol));

            // -- Check Time --
            if (Log::CheckTimeSeconds(5.)) {
              double progress = double(inputImg.GetIndex(x, y)) / double(inputImg.GetSize());
              progress *= 100.;

              Log::EndLine();
              Log::StartLine();
              Log::Write("  ");
              Log::Write(std::to_string(progress));
              Log::Write("%");

              Log::StartTime();
            }
          }
        }
        Log::EndLine();
        Log::EndLine();

        // -- Start --

        Log::StartLine();
        Log::Write("Processing...");

        Log::StartTime();
        for (int y = 0; y < inputImg.GetHeight(); y++) {
          for (int x = 0; x < inputImg.GetWidth(); x++) {
            // x + y * m_w
            //const size_t labI = size_t(x + y * inputImg.GetWidth());
            const size_t labI = GetIndex(x, y, inputImg.GetWidth());

            const OkLab oldPixelLab = pixelsLab[labI];
            const OkLab newPixelLab = ClosestPaletteColorLab(palette, oldPixelLab, dist_l);

            // -- Set New Pixel Colour --

            const sRGB newPixelRGB = OkLab::OkLabtosRGB(newPixelLab);
            SetDataFromRGB(inputImg, x, y, newPixelRGB);
            pixelsLab[labI] = newPixelLab;

            // -- Dither --

            //const bool manualOverride = true;

            if (dither) {
              const OkLab quant_error = oldPixelLab - newPixelLab;

              if (x + 1 < inputImg.GetWidth()) {
                pixelsLab[GetIndex(x + 1, y, inputImg.GetWidth())] += quant_error * (7. / 16.);
              }

              if (y + 1 < inputImg.GetHeight()) {
                if (x - 1 >= 0) {
                  pixelsLab[GetIndex(x - 1, y, inputImg.GetWidth())] += quant_error * (3. / 16.);
                }

                pixelsLab[GetIndex(x, y + 1, inputImg.GetWidth())] += quant_error * (5. / 16.);

                if (x + 1 < inputImg.GetWidth()) {
                  pixelsLab[GetIndex(x + 1, y + 1, inputImg.GetWidth())] += quant_error * (1. / 16.);
                }
              }
            }

            // -- Check Time --
            if (Log::CheckTimeSeconds(5.)) {
              double progress = double(inputImg.GetIndex(x, y)) / double(inputImg.GetSize());
              progress *= 100.;

              Log::EndLine();
              Log::StartLine();
              Log::Write("  ");
              Log::Write(std::to_string(progress));
              Log::Write("%");

              Log::StartTime();
            }
          }
        }
        Log::EndLine();
        Log::EndLine();
        
        inputImg.Write(outputLoc.c_str());
      }
    }
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
