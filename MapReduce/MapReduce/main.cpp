#include "Executive.h"
#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <Windows.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "mapper") {
        if (argc != 6) {
            std::cerr << "Error: Incorrect number of arguments for mapper mode.\n";
            return 1;
        }
        int mapperIndex = std::stoi(argv[2]);
        std::string mapDllPath = argv[3];
        std::string inputDirectory = argv[4];
        int numReducers = std::stoi(argv[5]);

        Executive exec(inputDirectory, "", "", mapDllPath, "", 1000, numReducers);
        exec.run("mapper", mapperIndex);
        return 0;
    }

    std::string inputLine;
    std::string inputDirectory, tempDirectory, outputDirectory;
    std::string mapDllPath, reduceDllPath;
    int bufferSize = 1000;
    int numReducers = 2;

    std::cout << "Enter the input file path, temporary directory, output directory, Map DLL path, and Reduce DLL path all separated by spaces:\n";
    std::cout << "Example: C:\\Path\\To\\InputFiles C:\\Path\\To\\Temp C:\\Path\\To\\Output C:\\Path\\To\\Map.dll C:\\Path\\To\\Reduce.dll\n";
    std::getline(std::cin, inputLine);

    std::istringstream iss(inputLine);
    if (!(iss >> inputDirectory >> tempDirectory >> outputDirectory >> mapDllPath >> reduceDllPath)) {
        std::cerr << "Error: You must enter exactly five paths separated by spaces." << std::endl;
        return 1;
    }

    if (!fs::exists(inputDirectory) || !fs::is_directory(inputDirectory)) {
        std::cerr << "Error: Input directory does not exist or is not accessible." << std::endl;
        return 1;
    }
    if (!fs::exists(mapDllPath)) {
        std::cerr << "Error: Map DLL path does not exist or is not accessible." << std::endl;
        return 1;
    }
    if (!fs::exists(reduceDllPath)) {
        std::cerr << "Error: Reduce DLL path does not exist or is not accessible." << std::endl;
        return 1;
    }

    Executive exec(inputDirectory, tempDirectory, outputDirectory, mapDllPath, reduceDllPath, bufferSize, numReducers);

    exec.run("controller");

    return 0;
}
