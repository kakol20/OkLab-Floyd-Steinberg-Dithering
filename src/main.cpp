// main.cpp : Defines the entry point for the application.
//

#include "main.h"

int main(int argc, char* argv[]) {
  OkLab::Initialise();

  Log::StartLine();
  Log::Write("Conversion Matrices:\n" + OkLab::DebugsRGBtoOkLabMats());
  Log::Write(OkLab::DebugOkLabtosRGBMats());
  Log::EndLine();

  std::ifstream f("settings.json");

  if (f) {
    json settings_json = json::parse(f);

    const bool dither = settings_json["dither"];
    const bool saveGray = settings_json["save_grayscale"];
    const bool convertGray = settings_json["convert_grayscale"];
    Log::StartLine();
    Log::Write("Dither: ");
    Log::Write(dither ? "TRUE\n" : "FALSE\n");

    Log::StartLine();
    Log::Write("Convert To Grayscale: ");
    Log::Write(convertGray ? "TRUE\n" : "FALSE\n");

    Log::StartLine();
    Log::Write("Save Grayscale Image: ");
    Log::Write(saveGray ? "TRUE\n" : "FALSE\n");

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
      Image originalImage;

      if (originalImage.Read(imageFileLoc.c_str())) {
        if (convertGray) {
          // Convert image to grayscale first
          Log::StartLine();
          Log::Write("Converting to Grayscale");
          auto start = std::chrono::high_resolution_clock::now();
          for (int y = 0; y < originalImage.GetHeight(); y++) {
            for (int x = 0; x < originalImage.GetWidth(); x++) {
              const size_t index = originalImage.GetIndex(x, y);

              sRGB rgb(sRGB::UInt8toDouble(originalImage.GetData(index + 0)),
                sRGB::UInt8toDouble(originalImage.GetData(index + 1)),
                sRGB::UInt8toDouble(originalImage.GetData(index + 2)));
              OkLab lab = OkLab::sRGBtoOkLab(rgb);
              lab.ConvertToGrayscale();

              rgb = OkLab::OkLabtosRGB(lab);

              originalImage.SetData(index + 0, rgb.GetRUInt());
              originalImage.SetData(index + 1, rgb.GetGUInt());
              originalImage.SetData(index + 2, rgb.GetBUInt());

              auto stop = std::chrono::high_resolution_clock::now();
              auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
              if (duration.count() >= 5000) {
                long double progress = long double(index) / long double(originalImage.GetSize());
                progress *= 100.;

                Log::EndLine();
                Log::StartLine();
                Log::Write("  ");
                Log::Write(std::to_string(progress));
                Log::Write("%");

                start = std::chrono::high_resolution_clock::now();
              }
            }
          }
          Log::EndLine();
          Log::StartLine();
          Log::Write("Done\n");

          if (saveGray) {
            originalImage.Write("data/grayscale.png");
          }
        }

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
            long double progress = long double(i) / long double(originalImage.GetSize());
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

        const bool clampValue = true;

        Log::StartLine();
        Log::Write("Dithering");
        start = std::chrono::high_resolution_clock::now();
        for (int y = 0; y < copyImage.GetHeight(); y++) {
          for (int x = 0; x < copyImage.GetWidth(); x++) {
            const size_t img_index = copyImage.GetIndex(x, y);
            const size_t pixel_index = Image::GetIndex_s(x, y, copyImage.GetWidth(), 1);

            const OkLab oldpixel = pixels[pixel_index];

            const OkLab newpixel = ClosestPaletteColorLAB(colours, oldpixel);
            sRGB newpixel_rgb = OkLab::OkLabtosRGB(newpixel);
            newpixel_rgb.Clamp();

            copyImage.SetData(img_index + 0, newpixel_rgb.GetRUInt());
            copyImage.SetData(img_index + 1, newpixel_rgb.GetGUInt());
            copyImage.SetData(img_index + 2, newpixel_rgb.GetBUInt());

            if (dither) {
              const OkLab quant_error = oldpixel - newpixel;

              if (x + 1 < copyImage.GetWidth()) {
                const size_t qe_7 = Image::GetIndex_s(x + 1, y, copyImage.GetWidth(), 1);
                pixels[qe_7] = pixels[qe_7] + quant_error * (7. / 16.);

                if (clampValue) pixels[qe_7].RGBClamp();
              }

              if (y + 1 < copyImage.GetHeight()) {
                if (x - 1 >= 0) {
                  const size_t qe_3 = Image::GetIndex_s(x - 1, y + 1, copyImage.GetWidth(), 1);
                  pixels[qe_3] = pixels[qe_3] + quant_error * (3. / 16.);

                  if (clampValue) pixels[qe_3].RGBClamp();
                }

                const size_t qe_5 = Image::GetIndex_s(x, y + 1, copyImage.GetWidth(), 1);
                pixels[qe_5] = pixels[qe_5] + quant_error * (5. / 16.);

                if (clampValue) pixels[qe_5].RGBClamp();

                if (x + 1 < copyImage.GetWidth()) {
                  const size_t qe_1 = Image::GetIndex_s(x + 1, y + 1, copyImage.GetWidth(), 1);
                  pixels[qe_1] = pixels[qe_1] + quant_error * (1. / 16.);

                  if (clampValue) pixels[qe_1].RGBClamp();
                }
              }
            }

            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            if (duration.count() >= 5000) {
              long double progress = long double(img_index) / long double(copyImage.GetSize());
              progress *= 100.;

              Log::EndLine();
              Log::StartLine();
              Log::Write("  ");
              Log::Write(std::to_string(progress));
              Log::Write("%");

              start = std::chrono::high_resolution_clock::now();
            }
          }
        }
        Log::EndLine();
        Log::StartLine();
        Log::Write("Done\n");

        std::string output = settings_json["output_image"];
        copyImage.Write(output.c_str());
      }
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
  long double closestDist = OkLab::SqrDist(col_lab, closest);

  for (size_t i = 1; i < palette.size(); i++) {
    long double dist = OkLab::SqrDist(col_lab, palette[i]);

    if (dist < closestDist) {
      closest = palette[i];
      closestDist = dist;
    }
  }

  return OkLab::OkLabtosRGB(closest);
}

OkLab ClosestPaletteColorLAB(const std::vector<OkLab>& palette, const OkLab& lab) {
  OkLab closest = palette[0];
  long double closestDist = OkLab::SqrDist(lab, closest);

  for (size_t i = 1; i < palette.size(); i++) {
    long double dist = OkLab::SqrDist(lab, palette[i]);

    if (dist < closestDist) {
      closest = palette[i];
      closestDist = dist;
    }
  }
  return closest;
}