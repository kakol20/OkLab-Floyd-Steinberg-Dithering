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

      for (int y = 0; y < copyImage.GetHeight(); y++) {
        for (int x = 0; x < copyImage.GetWidth(); x++) {
          const size_t index = inputImage.GetIndex(x, y);

          const sRGB oldpixel(double(copyImage.GetData(index + 0)) / 255.,
            double(copyImage.GetData(index + 1)) / 255.,
            double(copyImage.GetData(index + 2)) / 255.);

          const sRGB newPixel = ClosestPaletteColor(palettes[chosen.c_str()], oldpixel);
          copyImage.SetData(index + 0, newPixel.GetRUInt());
          copyImage.SetData(index + 1, newPixel.GetGUInt());
          copyImage.SetData(index + 2, newPixel.GetBUInt());

          if (dithered) {
            const sRGB quant_error = oldpixel - newPixel;

            if (y + 1 < copyImage.GetHeight()) {
              const size_t qe_5 = copyImage.GetIndex(x, y + 1);
              sRGB new_5(double(copyImage.GetData(qe_5 + 0)) / 255.,
                double(copyImage.GetData(qe_5 + 1)) / 255.,
                double(copyImage.GetData(qe_5 + 2)) / 255.);
              new_5 = new_5 + (quant_error * (5. / 16.));
              new_5.Clamp();
              copyImage.SetData(qe_5 + 0, new_5.GetRUInt());
              copyImage.SetData(qe_5 + 1, new_5.GetGUInt());
              copyImage.SetData(qe_5 + 2, new_5.GetBUInt());

              if (x + 1 < copyImage.GetWidth()) {
                const size_t qe_1 = copyImage.GetIndex(x + 1, y + 1);
                sRGB new_1(double(copyImage.GetData(qe_1 + 0)) / 255.,
                  double(copyImage.GetData(qe_1 + 1)) / 255.,
                  double(copyImage.GetData(qe_1 + 2)) / 255.);
                new_1 = new_1 + (quant_error * (1. / 16.));
                new_1.Clamp();
                copyImage.SetData(qe_1 + 0, new_1.GetRUInt());
                copyImage.SetData(qe_1 + 1, new_1.GetGUInt());
                copyImage.SetData(qe_1 + 2, new_1.GetBUInt());
              }

              if (x - 1 >= 0) {
                const size_t qe_3 = copyImage.GetIndex(x - 1, y + 1);
                sRGB new_3(double(copyImage.GetData(qe_3 + 0)) / 255.,
                  double(copyImage.GetData(qe_3 + 1)) / 255.,
                  double(copyImage.GetData(qe_3 + 2)) / 255.);
                new_3 = new_3 + (quant_error * (3. / 16.));
                new_3.Clamp();
                copyImage.SetData(qe_3 + 0, new_3.GetRUInt());
                copyImage.SetData(qe_3 + 1, new_3.GetGUInt());
                copyImage.SetData(qe_3 + 2, new_3.GetBUInt());
              }
            }

            if (x + 1 < copyImage.GetWidth()) {
              const size_t qe_7 = copyImage.GetIndex(x + 1, y);
              sRGB new_7(double(copyImage.GetData(qe_7 + 0)) / 255.,
                double(copyImage.GetData(qe_7 + 1)) / 255.,
                double(copyImage.GetData(qe_7 + 2)) / 255.);
              new_7 = new_7 + (quant_error * (7. / 16.));
              new_7.Clamp();
              copyImage.SetData(qe_7 + 0, new_7.GetRUInt());
              copyImage.SetData(qe_7 + 1, new_7.GetGUInt());
              copyImage.SetData(qe_7 + 2, new_7.GetBUInt());
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

sRGB ClosestPaletteColor(const std::vector<OkLab>& palette, const sRGB& col) {
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