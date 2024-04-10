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
        Log::Write(palettesJSON["chosen_palette"]);
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
    }

    Log::Save();

    std::cout << "Press enter to exit...\n";
    std::cin.ignore();
    return 0;
}