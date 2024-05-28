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
#include "colorSpaces/sRGB.hpp"
#include "maths/Maths.hpp"
#include "other/Log.h"

#include "image/Image.h"

sRGB ClosestPaletteColorRGB(const std::vector<sRGB>& palette, const sRGB& col);

bool GeneratePalette(const std::string& baseFile);

#endif // !MAIN_H
