//#include <OpenSim/Actuators/TorqueActuator.h>
//#include <OpenSim/Actuators/CoordinateActuator.h>
//#include <OpenSim/Common/STOFileAdapter.h>
//#include <OpenSim/Moco/osimMoco.h>
//
//using namespace OpenSim;
//using SimTK::Pi;
//using namespace std;
//
//int main() {
//	Model model("Models/Ref_sl.osim");
//    // Get the PhysicalOffsetFrame by name
//
//	const auto& jset = model.getJointSet();
//	auto& femur_weld_r = jset.get("femur_weld_r");
//	auto& tibial_plat_weld_r = jset.get("tibial_plat_weld_r");
//	auto& fem_offset = femur_weld_r.get_frames(1);
//	auto& tib_offset = tibial_plat_weld_r.get_frames(1);
//	auto& fem_rotation = fem_offset.get_orientation();
//	auto& tib_rotation = tib_offset.get_orientation();
//	const auto& fem = fem_rotation[1] * 180 / Pi;
//	const auto& tib = tib_rotation[0] * 180 / Pi;
//
//	cout << "a" << endl;
//
//	string fem_str = to_string(static_cast<int>(round(abs(fem))));
//	string tib_str = to_string(static_cast<int>(round(abs(tib))));
//
//	fem_str = string(2 - fem_str.length(), '0') + fem_str;
//	tib_str = string(2 - tib_str.length(), '0') + tib_str;
//
//	if (fem < 0) { fem_str = '-' + fem_str; } else { fem_str = fem_str; }
//	if (tib < 0) { tib_str = '-' + tib_str; } else { tib_str = tib_str; }
//
//	cout<<"b"<<endl;
//
//	//std::cout << "Predicted Maximum Flexion Angles" << endl;
//	//std::cout << " " << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//	//std::cout << "neutral" << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//	//std::cout << "tib_vv_05" << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//	//std::cout << "tib_vv_10" << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//	//std::cout << "tib_vv_15" << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//	//std::cout << "tib_vv_-05" << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//	//std::cout << "tib_vv_-10" << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//	//std::cout << "tib_vv_-15" << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
//
//
//	return 0;
//}
//
//
//
////int main() {
////    std::string directoryPath = "Results/CAMS_Knee"; // Replace with your directory path
////    std::vector<std::string> datasetFiles;
////
////    WIN32_FIND_DATA findFileData;
////    HANDLE hFind = FindFirstFile((directoryPath + "/*").c_str(), &findFileData);
////
////    if (hFind != INVALID_HANDLE_VALUE) {
////        do {
////            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
////                datasetFiles.push_back(directoryPath + "/" + findFileData.cFileName);
////            }
////        } while (FindNextFile(hFind, &findFileData) != 0);
////        FindClose(hFind);
////    }
////    else {
////        std::cerr << "Error opening directory" << std::endl;
////        return 1;
////    }
////
////    // Print the contents of the vector
////    for (const std::string& fileName : datasetFiles) {
////        std::cout << fileName << std::endl;
////    }
////
////    return 0;
////}
//
//
////using namespace std;
////
////#ifndef M_PI
////#define M_PI 3.14159265358979323846
////#endif
////
////// Model list
////std::string dataDir = "expBoundaryResult/"; // Directory path where your dataset files are stored
//////std::vector<std::string> datasetFiles = {
//////    dataDir + "K1L_prediction.sto",
//////    dataDir + "K2L_prediction.sto",
//////    dataDir + "K3R_prediction.sto",
//////    dataDir + "K5R_prediction.sto",
//////    dataDir + "K7L_prediction.sto",
//////    dataDir + "K8L_prediction.sto"
//////};
////
////std::vector<std::string> datasetFiles = {
////    "K1L_prediction.sto",
////    "K2L_prediction.sto",
////    "K3R_prediction.sto",
////    "K5R_prediction.sto",
////    "K7L_prediction.sto",
////    "K8L_prediction.sto"
////};
////
////// Function to split a string into tokens
////std::vector<std::string> splitString(const std::string& str, char delimiter) {
////    std::vector<std::string> tokens;
////    std::istringstream iss(str);
////    std::string token;
////    while (std::getline(iss, token, delimiter)) {
////        tokens.push_back(token);
////    }
////    return tokens;
////}
////
////// Function to extract the dataset name from the file name
////std::string extractDatasetName(const std::string& filename) {
////    // Find the position of the last slash in the filename
////    size_t lastSlashPos = filename.find_last_of("/");
////
////    // Find the position of "_prediction.sto" in the filename
////    size_t extensionPos = filename.find("_prediction.sto");
////
////    // Extract the dataset name between the last slash and "_prediction.sto"
////    std::string datasetName;
////    if (lastSlashPos != std::string::npos && extensionPos != std::string::npos) {
////        datasetName = filename.substr(lastSlashPos + 1, extensionPos - lastSlashPos - 1);
////    }
////
////    return datasetName;
////}
////
////int main() {
////    // Create a 2D vector to store the results
////    std::vector<std::vector<double>> resultArray(datasetFiles.size(), std::vector<double>(3, 0.0));
////    std::vector<std::vector<double>> diffArray(datasetFiles.size(), std::vector<double>(3, 0.0));
////
////    double expArray[][3] = {
////    {81.69399072, 70.65350934, 9.38602396},
////    {107.0939643, 92.95941183, 16.05635248},
////    {77.2417225, 71.67502451, 16.04911708},
////    {81.80970554, 92.36328771, 25.53585152},
////    {104.3089424, 80.57350014, 16.23760788},
////    {117.4530983, 87.46181932, 12.22363194}
////    };
////
////    //for (size_t i = 0; i < datasetFiles.size(); ++i) {
////    for (size_t i = 0; i < 6; ++i) {
////        const auto& datasetFile = datasetFiles[i];
////        std::string datasetName = extractDatasetName(datasetFile);
////
////        // Open the dataset file
////        std::ifstream inputFile(datasetFile);
////
////        if (!inputFile.is_open()) {
////            std::cout << "Failed to open the file." << std::endl;
////            return 1;
////        }
////
////        std::string line;
////        bool foundHeader = false;
////        int hipFlexionColumn = -1, kneeFlexionColumn = -1, ankleFlexionColumn = -1;
////        double maxHipFlexion = 0.0, maxKneeFlexion = 0.0, maxAnkleFlexion = 0.0;
////
////        // Read the file line by line
////        while (std::getline(inputFile, line)) {
////            if (!foundHeader) {
////                // Find the header row containing the desired columns
////                if (line.find("time") != std::string::npos) {
////                    std::vector<std::string> headers = splitString(line, '\t');
////
////                    // Find the column indices of the desired variables
////                    for (size_t i = 0; i < headers.size(); ++i) {
////                        if (headers[i] == "/jointset/hip_r/hip_flexion_r/value") {
////                            hipFlexionColumn = static_cast<int>(i);
////                        }
////                        else if (headers[i] == "/jointset/knee_r/knee_flexion_r/value") {
////                            kneeFlexionColumn = static_cast<int>(i);
////                        }
////                        else if (headers[i] == "/jointset/ankle_r/ankle_angle_r/value") {
////                            ankleFlexionColumn = static_cast<int>(i);
////                        }
////                    }
////
////                    foundHeader = true;
////                }
////            }
////            else {
////                // Find the maximum values in the desired columns
////                std::vector<std::string> data = splitString(line, '\t');
////
////                if (data.size() > std::max({ hipFlexionColumn, kneeFlexionColumn, ankleFlexionColumn })) {
////                    double hipFlexionValue = std::stod(data[hipFlexionColumn]);
////                    double kneeFlexionValue = std::stod(data[kneeFlexionColumn]);
////                    double ankleFlexionValue = std::stod(data[ankleFlexionColumn]);
////
////                    double hipFlexionDegrees = (hipFlexionValue * 180.0 / M_PI);
////                    double kneeFlexionDegrees = (kneeFlexionValue * 180.0 / M_PI);
////                    double ankleFlexionDegrees = (ankleFlexionValue * 180.0 / M_PI);
////
////                    maxHipFlexion = std::max(maxHipFlexion, hipFlexionDegrees);
////                    maxKneeFlexion = std::max(maxKneeFlexion, kneeFlexionDegrees);
////                    maxAnkleFlexion = std::max(maxAnkleFlexion, ankleFlexionDegrees);
////                }
////            }
////        }
////
////        // Store the maximum values in the result array
////        resultArray[i] = { maxHipFlexion, maxKneeFlexion, maxAnkleFlexion };
////
////        // Calculate the difference
////        for (int j = 0; j < 3; j++) {
////            diffArray[i][j] = resultArray[i][j] - expArray[i][j];
////        }
////
////        // Close the file
////        inputFile.close();
////    }
////
////    std::cout << "Experimental Maximum Flexion Angles" << std::endl;
////    std::cout << std::setw(10) << " " << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
////
////    //std::string datasetNames[] = { "Dataset 1", "Dataset 2", "Dataset 3", "Dataset 4", "Dataset 5", "Dataset 6" };
////
////    for (size_t i = 0; i < sizeof(expArray) / sizeof(expArray[0]); ++i) {
////        std::string datasetName = extractDatasetName(datasetFiles[i]);
////        std::cout << std::setw(10) << datasetName;
////        for (size_t j = 0; j < sizeof(expArray[0]) / sizeof(expArray[0][0]); ++j) {
////            std::cout << std::setw(10) << expArray[i][j];
////        }
////        std::cout << std::endl;
////    }
////
////    std::cout << "" << std::endl;
////    std::cout << "" << std::endl;
////
////    std::cout << "Predicted Maximum Flexion Angles" << std::endl;
////    std::cout << std::setw(10) << " " << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
////    for (size_t i = 0; i < resultArray.size(); ++i) {
////        std::string datasetName = extractDatasetName(datasetFiles[i]);
////        std::cout << std::setw(10) << datasetName;
////        for (size_t j = 0; j < resultArray[i].size(); ++j) {
////            std::cout << std::setw(10) << resultArray[i][j];
////        }
////        std::cout << std::endl;
////    }
////
////    std::cout << "" << std::endl;
////    std::cout << "" << std::endl;
////
////    std::cout << "Maximum Flexion Angle Differences: EXP VS PRED" << std::endl;
////    std::cout << std::setw(10) << " " << std::setw(10) << "hip" << std::setw(10) << "knee" << std::setw(10) << "ankle" << std::endl;
////    for (size_t i = 0; i < diffArray.size(); ++i) {
////        std::string datasetName = extractDatasetName(datasetFiles[i]);
////        std::cout << std::setw(10) << datasetName;
////        for (size_t j = 0; j < diffArray[i].size(); ++j) {
////            std::cout << std::setw(10) << diffArray[i][j];
////        }
////        std::cout << std::endl;
////    }
////
////    return 0;
////}

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::string directoryPath = "/path/to/your/directory";

    if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
        std::cout << "The directory exists." << std::endl;
    }
    else {
        std::cout << "The directory does not exist." << std::endl;
    }

    return 0;
}
