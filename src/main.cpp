// main.cpp : Defines the entry point for the application.
//

#include "main.h"

int main(int argc, char* argv[]) {
  OkLab::Initialise();

  std::ifstream f("palettes.json");

  if (f) {
    std::map<std::string, std::vector<OkLab>> palettes;
    json palettesJSON = json::parse(f);

    const auto& palettesStr = palettesJSON["palettes"];
    Log::StartLine();
    Log::Write("Chosen Palette: ");
    const std::string chosen = palettesJSON["chosen_palette"];
    Log::Write(chosen);
    Log::EndLine();

    for (auto& [key, val] : palettesStr.items()) {
      Log::StartLine();
      Log::Write(key + '\n');
      for (auto& colors : palettesStr[key]) {
        const auto& hex = colors.get<std::string>();
        const sRGB srgb = sRGB::HexTosRGB(hex);
        const OkLab lab = OkLab::sRGBtoOkLab(srgb);
        palettes[key].push_back(lab);

        Log::Write("  #" + hex + ", sRGB" + srgb.Output() + ", OkLab" + lab.Output() + '\n');
      }
    }
    Log::EndLine();

    // Load Image

    Image inputImage;
    const std::string imageFile = palettesJSON["input_image"];

    if (inputImage.Read(imageFile.c_str(), 3)) {
      Image copyImage(inputImage);

      const bool dithered = palettesJSON["dither"];

      std::vector<OkLab> pixels;
      pixels.reserve((size_t)copyImage.GetWidth() * (size_t)copyImage.GetHeight());

      for (size_t i = 0; i < copyImage.GetSize(); i += 3) {
        sRGB srgb(double(copyImage.GetData(i + 0)) / 255.,
          double(copyImage.GetData(i + 1)) / 255.,
          double(copyImage.GetData(i + 2)) / 255.);
        pixels.push_back(OkLab::sRGBtoOkLab(srgb));
      }

      bool clampValue = true;

      for (int y = 0; y < copyImage.GetHeight(); y++) {
        for (int x = 0; x < copyImage.GetWidth(); x++) {
          const size_t img_index = copyImage.GetIndex(x, y);
          const size_t pixel_index = Image::GetIndex_s(x, y, copyImage.GetWidth(), 1);

          const OkLab oldpixel = pixels[pixel_index];

          const OkLab newPixel = ClosestPaletteColorLAB(palettes[chosen.c_str()], oldpixel);
          const sRGB newPixel_rgb = OkLab::OkLabtosRGB(newPixel);
          copyImage.SetData(img_index + 0, newPixel_rgb.GetRUInt());
          copyImage.SetData(img_index + 1, newPixel_rgb.GetGUInt());
          copyImage.SetData(img_index + 2, newPixel_rgb.GetBUInt());

          if (dithered) {
            const OkLab quant_error = oldpixel - newPixel;

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
        }
      }

      const std::string outputImageFile = palettesJSON["output_image"];
      copyImage.Write(outputImageFile.c_str());
    }
  }

  Log::Save();

  std::cout << "Press enter to exit...\n";
  std::cin.ignore();
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
