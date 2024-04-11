// main.cpp : Defines the entry point for the application.
//

#include "main.h"

int main(int argc, char* argv[]) {
  OkLab::Initialise();

  std::ifstream f("settings.json");

  if (f) {
    json settings_json = json::parse(f);

    const bool dither = settings_json["dither"];
    Log::StartLine();
    Log::Write("Dither: ");
    Log::Write(dither ? "TRUE\n" : "FALSE\n");

    // Get colours from palette file

    std::vector<OkLab> colours;
    std::fstream paletteFile;
    std::string paletteFileLoc = settings_json["palette"];

    paletteFile.open(paletteFileLoc);

    if (paletteFile) {
      Log::StartLine();

      Log::Write(paletteFileLoc + '\n');

      std::string line;
      while (std::getline(paletteFile, line)) {
        sRGB srgb = sRGB::HexTosRGB(line);
        OkLab lab = OkLab::sRGBtoOkLab(srgb);

        Log::Write("  #" + line + ", sRGB" + srgb.Output() + ", OkLab" + lab.Output() + '\n');

        colours.push_back(lab);
      }

      std::string imageFileLoc = settings_json["input_image"];
      const Image originalImage(imageFileLoc.c_str(), 3);
      Image copyImage = originalImage;

      // Replicate image into array of OkLab

      std::vector<OkLab> pixels;
      const size_t pixels_size = (size_t)originalImage.GetWidth() * (size_t)originalImage.GetHeight();
      pixels.reserve(pixels_size);

      Log::StartLine();
      Log::Write("Processing Pixels");
      auto start = std::chrono::high_resolution_clock::now();

      for (size_t i = 0; i < originalImage.GetSize(); i += 3) {
        sRGB srgb(sRGB::UInt8toDouble(originalImage.GetData(i + 0)),
          sRGB::UInt8toDouble(originalImage.GetData(i + 1)),
          sRGB::UInt8toDouble(originalImage.GetData(i + 2)));

        pixels.push_back(OkLab::sRGBtoOkLab(srgb));

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        if (duration.count() >= 5000) {
          double progress = double(i) / double(originalImage.GetSize());
          progress *= 100.;

          Log::EndLine();
          Log::StartLine();
          Log::Write("  ");
          Log::Write(std::to_string(progress));
          Log::Write("%");

          start = std::chrono::high_resolution_clock::now();
        }
      }
      Log::EndLine();
      Log::StartLine();
      Log::Write("Done\n");

      // ----- MAIN DITHER -----
    }
    else {
      Log::StartLine();
      Log::Write("Failed to read: ");
      Log::Write(paletteFileLoc);
      Log::EndLine();
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

sRGB ClosestPaletteColorRGB(const std::vector<OkLab>& palette, const sRGB& col) {
  const OkLab col_lab = OkLab::sRGBtoOkLab(col);
  OkLab closest = palette[0];
  double closestDist = OkLab::SqrDist(col_lab, closest);

  for (size_t i = 1; i < palette.size(); i++) {
    double dist = OkLab::SqrDist(col_lab, palette[i]);

    if (dist < closestDist) {
      closest = palette[i];
      closestDist = dist;
    }
  }

  return OkLab::OkLabtosRGB(closest);
}

OkLab ClosestPaletteColorLAB(const std::vector<OkLab>& palette, const OkLab& lab) {
  OkLab closest = palette[0];
  double closestDist = OkLab::SqrDist(lab, closest);

  for (size_t i = 1; i < palette.size(); i++) {
    double dist = OkLab::SqrDist(lab, palette[i]);

    if (dist < closestDist) {
      closest = palette[i];
      closestDist = dist;
    }
  }
  return closest;
}
