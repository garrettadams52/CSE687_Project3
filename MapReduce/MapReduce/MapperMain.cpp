#include <iostream>
#include <windows.h>
#include "IMap.h"
#include "IFileManagement.h"
#include "FileManagement.h"

typedef IMap* (*CREATEMAPFUNC)(IFileManagement*, size_t, int);

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: MapperMain <mapper_id> <input_directory> <temp_directory> <map_dll_path>" << std::endl;
        return 1;
    }

    int mapperId = std::stoi(argv[1]);
    std::string inputDirectory = argv[2];
    std::string tempDirectory = argv[3];
    std::string mapDllPath = argv[4];

    // Load the DLL
    HMODULE hMapDll = LoadLibraryA(mapDllPath.c_str());
    if (!hMapDll) {
        std::cerr << "Failed to load Map DLL: " << mapDllPath << std::endl;
        return 1;
    }

    // Get the CreateMapInstance function
    CREATEMAPFUNC createMap = (CREATEMAPFUNC)GetProcAddress(hMapDll, "CreateMapInstance");
    if (!createMap) {
        std::cerr << "Failed to find CreateMapInstance function in DLL: " << mapDllPath << std::endl;
        FreeLibrary(hMapDll);
        return 1;
    }

    // Create file management object
    FileManagement fileManagement(inputDirectory, tempDirectory, tempDirectory);

    // Create the mapper instance
    IMap* mapInstance = createMap(&fileManagement, 1000, 3);
    if (!mapInstance) {
        std::cerr << "Failed to create map instance" << std::endl;
        FreeLibrary(hMapDll);
        return 1;
    }

    // Run the map function for each file in the input directory
    for (const auto& file : fileManagement.getAllFiles()) {
        auto lines = fileManagement.readFile(file);
        for (const auto& line : lines) {
            mapInstance->MapFunction(file, line);
        }
    }

    // Clean up
    delete mapInstance;
    FreeLibrary(hMapDll);

    return 0;
}
