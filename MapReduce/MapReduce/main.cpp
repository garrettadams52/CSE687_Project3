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
    int bufferSize = 1000;    // Buffer size for the mapper
    int numReducers = 4;      // Number of reducer instances, adjust as needed

    // Prompt the user for input paths and read them from standard input
    std::cout << "Enter the input file path, temporary directory, output directory, Map DLL path, and Reduce DLL path all separated by spaces:\n";
    std::cout << "Example: C:\\Path\\To\\InputFiles C:\\Path\\To\\Temp C:\\Path\\To\\Output C:\\Path\\To\\Map.dll C:\\Path\\To\\Reduce.dll\n";
    std::getline(std::cin, inputLine);

    // Parse the input line into individual path variables
    std::istringstream iss(inputLine);
    if (!(iss >> inputDirectory >> tempDirectory >> outputDirectory >> mapDllPath >> reduceDllPath)) {
        std::cerr << "Error: You must enter exactly five paths separated by spaces." << std::endl;
        return 1;   // Exit if the input is not correctly formatted
    }

    // Check if the specified directories and files exist and are accessible
    if (!fs::exists(inputDirectory) || !fs::is_directory(inputDirectory) ||
        !fs::exists(tempDirectory) || !fs::is_directory(tempDirectory) ||
        !fs::exists(outputDirectory) || !fs::is_directory(outputDirectory) ||
        !fs::exists(mapDllPath) || !fs::exists(reduceDllPath)) {
        std::cerr << "Error: One or more specified paths do not exist or are not accessible." << std::endl;
        return 1;   // Exit if any of the paths are invalid
    }

    // Create an instance of the Executive class with the provided paths and parameters
    Executive exec(inputDirectory, tempDirectory, outputDirectory, mapDllPath, reduceDllPath, bufferSize, numReducers);

    // Run the MapReduce process and handle any exceptions that may occur
    try {
        exec.run();
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;   // Exit if an exception is thrown
    }

    

    return 0;   // Exit successfully
}
