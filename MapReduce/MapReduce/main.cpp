#include "Executive.h"
#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <Windows.h>

namespace fs = std::filesystem;

int main() {
    std::string inputLine;
    std::string inputDirectory, tempDirectory, outputDirectory;
    std::string mapDllPath, reduceDllPath;
    int bufferSize = 1000;
    int numReducers = 4;  // Adjust as needed

    std::cout << "Enter the input file path, temporary directory, output directory, Map DLL path, and Reduce DLL path all separated by spaces:\n";
    std::cout << "Example: C:\\Path\\To\\InputFiles C:\\Path\\To\\Temp C:\\Path\\To\\Output C:\\Path\\To\\Map.dll C:\\Path\\To\\Reduce.dll\n";
    std::getline(std::cin, inputLine);

    std::istringstream iss(inputLine);
    if (!(iss >> inputDirectory >> tempDirectory >> outputDirectory >> mapDllPath >> reduceDllPath)) {
        std::cerr << "Error: You must enter exactly five paths separated by spaces." << std::endl;
        return 1;
    }

    if (!fs::exists(inputDirectory) || !fs::is_directory(inputDirectory) ||
        !fs::exists(tempDirectory) || !fs::is_directory(tempDirectory) ||
        !fs::exists(outputDirectory) || !fs::is_directory(outputDirectory) ||
        !fs::exists(mapDllPath) || !fs::exists(reduceDllPath)) {
        std::cerr << "Error: One or more specified paths do not exist or are not accessible." << std::endl;
        return 1;
    }

    Executive exec(inputDirectory, tempDirectory, outputDirectory, mapDllPath, reduceDllPath, bufferSize, numReducers);

    try {
        exec.run();
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
