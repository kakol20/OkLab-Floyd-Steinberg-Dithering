#pragma once

#include <string>

class Log {
public:
  Log() {};
  ~Log() {};

  static void Write(const std::string input);

  static void EndLine();
  static void StartLine();

  static void Save(const bool overwrite = true);

private:
  static std::string Console;
};