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

#include "colorSpaces/ColorSpace.h"
#include "colorSpaces/OkLab.h"
#include "colorSpaces/sRGB.h"
#include "maths/Maths.hpp"
#include "maths/Matrix.h"
#include "other/Log.h"

#include "image/Image.h"

sRGB ClosestPaletteColorRGB(const std::vector<OkLab>& palette, const sRGB& col);

OkLab ClosestPaletteColorLAB(const std::vector<OkLab>& palette, const OkLab& lab);

#endif // !MAIN_H
