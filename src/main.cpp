// main.cpp : Defines the entry point for the application.
//

#include "main.h"

int main(int argc, char* argv[]) {
    //std::cout << "Hello World\n";

    /*Log::StartLine();
    Log::Write("Hello World!");
    Log::EndLine();

    Log::Save();*/

    std::ifstream f("palettes.json");


    if (f) {
        std::map<std::string, std::vector<std::string>> palettes;
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
                palettes[key].push_back(hex);

                Log::Write("  #" + hex + '\n');
            }
        }
    }

    Log::Save();

    std::cout << "Press enter to exit...\n";
    std::cin.ignore();
    return 0;
}