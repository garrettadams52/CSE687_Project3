#include "Controller.h"
#include "Executive.h"
#include "FileManagement.h"
#include "IMap.h"
#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

void runMapper(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: MapReduce.exe mapper <mapper_id> <input_directory> <temp_directory> <map_dll_path>" << std::endl;
        return;
    }

    int mapperId = std::stoi(argv[2]);
    std::string inputDirectory = argv[3];
    std::string tempDirectory = argv[4];
    std::string mapDllPath = argv[5];

    HMODULE hMapDll = LoadLibraryA(mapDllPath.c_str());
    if (!hMapDll) {
        std::cerr << "Failed to load Map DLL: " << mapDllPath << std::endl;
        return;
    }

    typedef IMap* (*CREATEMAPFUNC)(IFileManagement*, size_t, int);
    CREATEMAPFUNC createMap = (CREATEMAPFUNC)GetProcAddress(hMapDll, "CreateMapInstance");
    if (!createMap) {
        std::cerr << "Failed to find CreateMapInstance function in DLL: " << mapDllPath << std::endl;
        FreeLibrary(hMapDll);
        return;
    }

    FileManagement fileManagement(inputDirectory, tempDirectory, tempDirectory);
    IMap* mapInstance = createMap(&fileManagement, 1000, 3);
    if (!mapInstance) {
        std::cerr << "Failed to create map instance" << std::endl;
        FreeLibrary(hMapDll);
        return;
    }

    for (const auto& file : fileManagement.getAllFiles()) {
        auto lines = fileManagement.readFile(file);
        for (const auto& line : lines) {
            mapInstance->MapFunction(file, line);
        }
    }

    delete mapInstance;
    FreeLibrary(hMapDll);
    std::cout << "Mapper process completed" << std::endl;
}

void runController() {
    std::string inputLine;
    std::string inputDirectory, tempDirectory, outputDirectory;
    std::string mapDllPath, reduceDllPath;
    int bufferSize = 1000;
    int numReducers = 3;
    int numMappers = numReducers + 1;

    while (true) {
        std::cout << "Enter the input file path, temporary directory, output directory, Map DLL path, and Reduce DLL path all separated by spaces:\n";
        std::cout << "Example: C:\\Path\\To\\InputFiles C:\\Path\\To\\Temp C:\\Path\\To\\Output C:\\Path\\To\\Map.dll C:\\Path\\To\\Reduce.dll\n";
        std::getline(std::cin, inputLine);

        std::istringstream iss(inputLine);
        if (!(iss >> inputDirectory >> tempDirectory >> outputDirectory >> mapDllPath >> reduceDllPath)) {
            std::cerr << "Error: You must enter exactly five paths separated by spaces." << std::endl;
            continue;
        }

        if (!fs::exists(inputDirectory) || !fs::is_directory(inputDirectory) ||
            !fs::exists(tempDirectory) || !fs::is_directory(tempDirectory) ||
            !fs::exists(outputDirectory) || !fs::is_directory(outputDirectory) ||
            !fs::exists(mapDllPath) || !fs::exists(reduceDllPath)) {
            std::cerr << "Error: One or more specified paths do not exist or are not accessible." << std::endl;
            continue;
        }

        break;
    }

    Controller controller;
    controller.runMappers(numMappers, inputDirectory, tempDirectory, mapDllPath);

    Executive exec(inputDirectory, tempDirectory, outputDirectory, mapDllPath, reduceDllPath, bufferSize, numReducers);

    

    std::cout << "MapReduce job completed. Check the output directory for results." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "mapper") {
        runMapper(argc, argv);
    }
    else {
        runController();
    }

    return 0;
}
