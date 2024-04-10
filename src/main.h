// main.h : Include file for standard system include files,
// or project specific include files.

#ifndef MAIN_H
#define MAIN_H

//#define USE_MAP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "../ext/json/json.hpp"
using json = nlohmann::json;

#include "other/Log.h"
#include "maths/Matrix.h"
#include "colorSpaces/ColorSpace.h"
#include "colorSpaces/sRGB.h"

#endif // !MAIN_H
