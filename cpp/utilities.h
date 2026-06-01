#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


std::vector<std::string> splitString(const std::string& str, char delimiter);
std::string extractDatasetName(const std::string& filename);
std::string formatNumber(double value, int width);
std::string formatNumberDec(double value, int width, int precision);
