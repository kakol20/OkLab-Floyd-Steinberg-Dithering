﻿#include "main.h"

const double Maths::Pi = 3.1415926535;
const double Maths::Tau = 6.283185307;
const double Maths::RadToDeg = 180. / Maths::Pi;
const double Maths::DegToRad = Maths::Pi / 180.;

int main(int argc, char* argv[]) {
	if (argc < 4) {
		Log::WriteOneLine("Drag and drop an image file, a .palette file and a .json file");
		Log::WriteOneLine("Note: Only PNG, JPG, BMP or TGA image files are supported");

		Log::Save();
		std::cout << "\nPress enter to exit...\n";
		std::cin.ignore();
		return 0;
	}

	std::string imgLoc;
	bool haveImg = false;

	std::string jsonLoc;
	bool haveJson = false;

	std::string paletteLoc;
	bool havePalette = false;

	// ----- GET FILES -----

	for (int i = 1; i < argc; i++) {
		std::string fileExtension = GetFileExtension(argv[i]);

		if (fileExtension == "json") {
			jsonLoc = argv[i];
			haveJson = true;
		} else if (fileExtension == "palette") {
			paletteLoc = argv[i];
			havePalette = true;
		} else if (fileExtension == "png" || fileExtension == "jpg" || fileExtension == "bmp" || fileExtension == "tga") {
			imgLoc = argv[i];
			haveImg = true;
		}
	}

	// ----- CHECK IF HAVE ALL FILES -----

	if (!(haveImg && haveJson && havePalette)) {
		if (!haveImg) Log::WriteOneLine("Image file not found");
		if (!haveJson) Log::WriteOneLine("JSON file not found");
		if (!havePalette) Log::WriteOneLine("Palette file not found");

		Log::Save();
		std::cout << "\nPress enter to exit...\n";
		std::cin.ignore();
		return 0;
	}

	Log::WriteOneLine("Image:   " + imgLoc);
	Log::WriteOneLine("JSON:    " + jsonLoc);
	Log::WriteOneLine("Palette: " + paletteLoc);
	Log::EndLine();

	// ----- READ FILES -----

	Image inputImg;
	if (!inputImg.Read(imgLoc.c_str())) {
		Log::Save();
		std::cout << "\nPress enter to exit...\n";
		std::cin.ignore();
		return 0;
	}

	std::ifstream f(jsonLoc);
	if (!(f)) {
		Log::WriteOneLine("Read failed: " + jsonLoc);
		Log::Save();
		std::cout << "\nPress enter to exit...\n";
		std::cin.ignore();
		return 0;
	}
	Log::EndLine();

	json settings = json::parse(f);

	const bool haveDither = settings.contains("dither");
	const bool haveGrayscale = settings.contains("grayscale");
	const bool haveDist_lightness = settings.contains("dist_lightness");

	if (!(haveDither && haveGrayscale && haveDist_lightness)) {
		if (!haveDither) Log::WriteOneLine("JSON setting not found: dither");
		if (!haveGrayscale) Log::WriteOneLine("JSON setting not found: grayscale");
		if (!haveDist_lightness) Log::WriteOneLine("JSON setting not found: dist_lightness");

		Log::Save();
		std::cout << "\nPress enter to exit...\n";
		std::cin.ignore();
		return 0;
	}

	const bool dither = settings["dither"];
	const bool grayscale = settings["grayscale"];
	const bool dist_lightness = settings["dist_lightness"];

	Log::WriteOneLine("Dither: " + (std::string)(dither ? "TRUE" : "FALSE"));
	Log::WriteOneLine("Process As Grayscale: " + (std::string)(grayscale ? "TRUE" : "FALSE"));
	Log::WriteOneLine("Use Lightness As Distance: " + (std::string)(dist_lightness ? "TRUE" : "FALSE"));
	//Log::EndLine();

	std::vector<sRGB> palette;
	if (!GetPalette(paletteLoc, palette)) {
		Log::Save();
		std::cout << "\nPress enter to exit...\n";
		std::cin.ignore();
		return 0;
	}

	// ----- MAIN PROCESS -----

	Log::EndLine();

	Log::WriteOneLine("Width: " + std::to_string(inputImg.GetWidth()));
	Log::WriteOneLine("Height: " + std::to_string(inputImg.GetHeight()));

	std::vector<OkLab> pixelsLab;
	pixelsLab.reserve((size_t)(inputImg.GetWidth() * inputImg.GetHeight()));

	// -- Copy Pixels into vector of OkLab --

	Log::StartLine();
	Log::Write("Copying pixels...");

	Log::StartTime();
	for (int y = 0; y < inputImg.GetHeight(); y++) {
		for (int x = 0; x < inputImg.GetWidth(); x++) {
			const sRGB pixelCol = GetRGBFromImage(inputImg, x, y);
			const OkLab mult = OkLab(1., grayscale ? 0. : 1., grayscale ? 0. : 1.);
			pixelsLab.push_back(OkLab::sRGBtoOkLab(pixelCol) * mult);

			// -- Check Time --
			if (Log::CheckTimeSeconds(5.)) {
				double progress = double(inputImg.GetIndex(x, y)) / double(inputImg.GetSize());
				progress *= 100.;

				std::stringstream outStr;
				outStr << std::fixed << std::setprecision(6);

				if (progress < 10.) {
					outStr << " ";
				}
				outStr << progress;

				Log::EndLine();
				Log::StartLine();
				Log::Write("  ");
				Log::Write(outStr.str());
				Log::Write("%");

				Log::StartTime();
			}
		}
	}
	Log::EndLine();
	Log::EndLine();

	// -- Start --

	Log::StartLine();
	Log::Write("Processing...");

	Log::StartTime();
	for (int y = 0; y < inputImg.GetHeight(); y++) {
		for (int x = 0; x < inputImg.GetWidth(); x++) {
			const size_t labI = GetIndex(x, y, inputImg.GetWidth());

			const OkLab oldPixelLab = pixelsLab[labI];
			const OkLab newPixelLab = ClosestPaletteColorLab(palette, oldPixelLab, dist_lightness);

			// -- Set New Pixel Colour --

			const sRGB newPixelRGB = OkLab::OkLabtosRGB(newPixelLab);
			SetDataFromRGB(inputImg, x, y, newPixelRGB);
			pixelsLab[labI] = newPixelLab;

			// -- Dither --

			if (dither) {
				const OkLab quant_error = oldPixelLab - newPixelLab;

				if (x + 1 < inputImg.GetWidth()) {
					pixelsLab[GetIndex(x + 1, y, inputImg.GetWidth())] += quant_error * (7. / 16.);
				}

				if (y + 1 < inputImg.GetHeight()) {
					if (x - 1 >= 0) {
						pixelsLab[GetIndex(x - 1, y, inputImg.GetWidth())] += quant_error * (3. / 16.);
					}

					pixelsLab[GetIndex(x, y + 1, inputImg.GetWidth())] += quant_error * (5. / 16.);

					if (x + 1 < inputImg.GetWidth()) {
						pixelsLab[GetIndex(x + 1, y + 1, inputImg.GetWidth())] += quant_error * (1. / 16.);
					}
				}
			}

			// -- Check Time --

			if (Log::CheckTimeSeconds(5.)) {
				double progress = double(inputImg.GetIndex(x, y)) / double(inputImg.GetSize());
				progress *= 100.;

				std::stringstream outStr;
				outStr << std::fixed << std::setprecision(6);

				if (progress < 10.) {
					outStr << " ";
				}
				outStr << progress;

				Log::EndLine();
				Log::StartLine();
				Log::Write("  ");
				Log::Write(outStr.str());
				Log::Write("%");

				Log::StartTime();
			}
		}
	}
	Log::EndLine();
	Log::EndLine();

	const std::string outLoc = GetFileNoExtension(imgLoc) + "_dithered.png";
	inputImg.Write(outLoc.c_str());

	Log::Save();
	return 0;
}

sRGB ClosestPaletteColorRGB(const std::vector<sRGB>& palette, const sRGB& col, const bool lightMode) {
	const OkLab col_lab = OkLab::sRGBtoOkLab(col);

	size_t closest_index = 0;
	double closestDist = OkLab::Distance(col_lab, OkLab::sRGBtoOkLab(palette[closest_index]), lightMode);

	for (size_t i = 1; i < palette.size(); i++) {
		const OkLab currLab = OkLab::sRGBtoOkLab(palette[i]);
		const double currDist = OkLab::Distance(col_lab, currLab, lightMode);

		if (currDist < closestDist) {
			closest_index = i;
			closestDist = currDist;
		}
	}

	return palette[closest_index];
}

OkLab ClosestPaletteColorLab(const std::vector<sRGB>& palette, const OkLab& col, const bool lightMode) {
	size_t closest_index = 0;
	double closestDist = OkLab::Distance(col, OkLab::sRGBtoOkLab(palette[closest_index]), lightMode);

	for (size_t i = 1; i < palette.size(); i++) {
		const OkLab currLab = OkLab::sRGBtoOkLab(palette[i]);
		const double currDist = OkLab::Distance(col, currLab, lightMode);

		if (currDist < closestDist) {
			closest_index = i;
			closestDist = currDist;
		}
	}

	return OkLab::sRGBtoOkLab(palette[closest_index]);
}

bool GetPalette(const std::string& loc, std::vector<sRGB>& out) {
	std::fstream p(loc);
	if (p) {
		/*Log::StartLine();
		Log::Write("----- COLOURS -----");*/
		Log::EndLine();

		std::string hex;
		while (std::getline(p, hex)) {
			const sRGB rgb = sRGB::HexTosRGB(hex);
			const OkLab lab = OkLab::sRGBtoOkLab(rgb);
			const OkLCh lch = OkLCh::OkLabtoOkLCh(lab);

			out.push_back(rgb);

			std::string hexOut = "#" + hex;
			std::string rgbOut = "rgb(" + rgb.UintDebug() + ")";
			std::string labOut = "oklab(" + lab.Debug() + ")";
			std::string lchOut = "oklch(" + lch.Debug() + ")";

			Log::WriteOneLine(hexOut + " - " + rgbOut + " - " + labOut + " - " + lchOut);
		}

		Log::WriteOneLine("Read success: " + loc);

		return true;
	} else {
		Log::StartLine();
		Log::Write("Read failed: ");
		Log::Write(loc);
		Log::EndLine();

		p.close();
		return false;
	}
}

bool GeneratePalette(const std::string& baseFile) {
	const std::string baseFileLoc = baseFile + "_b.palette";
	const std::string flatFileLoc = baseFile + "_f.palette";
	const std::string stairFileLoc = baseFile + "_sc.palette";

	std::fstream bFile(baseFileLoc);

	if (bFile) {
		Log::StartLine();
		Log::Write(baseFileLoc);
		Log::Write(" opened");
		Log::EndLine();

		std::fstream fFile;
		fFile.open(flatFileLoc, std::ios_base::out);

		std::fstream scFile;
		scFile.open(stairFileLoc, std::ios_base::out);

		std::string line;
		while (std::getline(bFile, line)) {
			const sRGB high = sRGB::HexTosRGB(line);
			const sRGB low = high * 0.71;
			const sRGB mid = high * 0.86;

			fFile << mid.sRGBtoHex() + "\n";

			scFile << low.sRGBtoHex() + "\n" + mid.sRGBtoHex() + "\n" + high.sRGBtoHex() + "\n";
		}

		fFile.close();
		scFile.close();
	} else {
		Log::StartLine();
		Log::Write(baseFileLoc);
		Log::Write(" not found");
		Log::EndLine();
	}
	bFile.close();

	return false;
}

sRGB GetRGBFromImage(const Image& img, const int x, const int y, const bool grayscale) {
	const size_t index = img.GetIndex(x, y);

	if (img.GetChannels() == 1 || img.GetChannels() == 2) {
		const double v = sRGB::UInt8ToDouble(img.GetData(index));
		return sRGB(v, v, v);
	} else {
		const double r = sRGB::UInt8ToDouble(img.GetData(index + 0));
		const double g = sRGB::UInt8ToDouble(img.GetData(index + 1));
		const double b = sRGB::UInt8ToDouble(img.GetData(index + 2));

		if (grayscale) {
			const OkLab lab = OkLab::sRGBtoOkLab(sRGB(r, g, b)) * OkLab(1., 0., 0.);
			return OkLab::OkLabtosRGB(lab);
		} else {
			return sRGB(r, g, b);
		}
	}
}

void SetDataFromRGB(Image& img, const int x, const int y, const sRGB& srgb) {
	const size_t index = img.GetIndex(x, y);
	if (img.GetChannels() == 1 || img.GetChannels() == 2) {
		const OkLab lab = OkLab::sRGBtoOkLab(srgb) * OkLab(1., 0., 0.);
		const sRGB gray = OkLab::OkLabtosRGB(lab);

		const uint8_t v = gray.GetRUInt();

		img.SetData(index, v);
	} else {
		const uint8_t r = srgb.GetRUInt();
		const uint8_t g = srgb.GetGUInt();
		const uint8_t b = srgb.GetBUInt();

		img.SetData(index + 0, r);
		img.SetData(index + 1, g);
		img.SetData(index + 2, b);
	}
}

size_t GetIndex(const int x, const int y, const int width, const int channels) {
	// size_t((x + y * m_w) * m_channels)
	return size_t((x + y * width) * channels);
}

std::string GetFileExtension(const std::string loc) {
	std::stringstream locStream(loc);
	std::string locSeg;
	std::vector<std::string> locSegList;

	while (std::getline(locStream, locSeg, '.')) locSegList.push_back(locSeg);

	return locSegList.back();
}

std::string GetFileNoExtension(const std::string loc) {
	std::stringstream locStream(loc);
	std::string locSeg;
	std::vector<std::string> locSegList;

	while (std::getline(locStream, locSeg, '.')) locSegList.push_back(locSeg);

	std::string out = "";
	for (size_t i = 0; i < locSegList.size() - 1; i++) {
		if (i != 0) out += ".";
		out += locSegList[i];
	}

	return out;
}
