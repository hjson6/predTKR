#include "utilities.h"

using namespace std;
namespace fs = std::filesystem;

// Function to split a string into tokens
vector<string> splitString(const string& str, char delimiter) {
	vector<string> tokens;
	istringstream iss(str);
	string token;
	while (getline(iss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

// Function to extract the dataset name from the file name
string extractDatasetName(const string& filename) {
	string datasetName = filename;
	size_t extensionPos = datasetName.find("_prediction.sto");
	if (extensionPos != string::npos) {
		datasetName = datasetName.substr(0, extensionPos);
	}

	return datasetName;
}

// Utility function to format a number with leading zeros
std::string formatNumber(double value, int width) {
	std::ostringstream oss;
	oss << std::fixed << std::setfill('0') << std::setw(width) << static_cast<int>(value);
	return oss.str();
}


std::string formatNumberDec(double value, int width, int precision) {
	std::ostringstream oss;
	oss << std::fixed << std::setfill('0') << std::setw(width) << std::setprecision(precision) << value;
	return oss.str();
}
