// main.h : Include file for standard system include files,
// or project specific include files.

#ifndef MAIN_H
#define MAIN_H

//#define USE_MAP

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../ext/json/json.hpp"
using json = nlohmann::json;

#include "colorSpaces/ColorSpace.hpp"
#include "colorSpaces/OkLab.h"
#include "colorSpaces/OkLCh.h"
#include "colorSpaces/sRGB.hpp"
#include "maths/Maths.hpp"
#include "other/Log.h"

#include "image/Image.h"

sRGB ClosestPaletteColorRGB(const std::vector<sRGB>& palette, const sRGB& col, const bool lightMode = false);
OkLab ClosestPaletteColorLab(const std::vector<sRGB>& palette, const OkLab& col, const bool lightMode = false);
bool GetPalette(const std::string& loc, std::vector<sRGB>& out);

bool GeneratePalette(const std::string& baseFile);

sRGB GetRGBFromImage(const Image& img, const int x, const int y, const bool grayscale = false);
void SetDataFromRGB(Image& img, const int x, const int y, const sRGB& srgb);

size_t GetIndex(const int x, const int y, const int width, const int channels = 1);

#endif // !MAIN_H
