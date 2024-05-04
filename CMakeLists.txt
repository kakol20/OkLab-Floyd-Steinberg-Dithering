# CMakeList.txt : CMake project for OkLab Floyd-Steinberg Dithering, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("OkLab Floyd-Steinberg Dithering")

# set(OkLabDithering "OkLab Floyd-Steinberg Dithering")

# Add source to this project's executable.
add_executable (OkLabDithering "src/main.h" 
    "src/main.cpp"
    "ext/json/json.hpp"
    "ext/stb/stb_image.h"
    "ext/stb/stb_image_write.h"
    "src/image/Image.cpp"
    "src/other/Log.cpp"
    "src/maths/Matrix.cpp" 
    "src/colorSpaces/ColorSpace.h"
    "src/colorSpaces/sRGB.cpp"
    "src/colorSpaces/OkLab.cpp"
    "src/other/Pseudo2DArray.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET OkLabDithering PROPERTY CXX_STANDARD 20)
endif()