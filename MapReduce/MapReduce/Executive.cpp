#include "Executive.h"
#include <iostream>
#include <fstream>
#include <Windows.h>

Executive::Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
    const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize, int numReducers)
    : fileManagement(inputDir, tempDir, outputDir),
    hMapDll(nullptr), hReduceDll(nullptr),
    bufferSize(bufSize),
    numReducers(numReducers),
    workflow(inputDir, tempDir, outputDir, std::vector<IMap*>(), nullptr) {  // Temporarily initialize with an empty vector

    loadMapDll(mapDllPath);
    loadReduceDll(reduceDllPath);

    if (!mapInstances.empty() && reduceInstance) {
        workflow = Workflow(inputDir, tempDir, outputDir, mapInstances, reduceInstance);
    }
    else {
        std::cerr << "Failed to load map or reduce DLLs correctly.\n";
    }
}

void Executive::run() {
    if (!mapInstances.empty() && reduceInstance) {
        workflow.run();
    }
    else {
        std::cerr << "Map or Reduce instances are not initialized.\n";
    }
    markSuccess();
}

Executive::~Executive() {
    for (auto mapInstance : mapInstances) {
        delete mapInstance;
    }
    if (hMapDll) {
        FreeLibrary(hMapDll);
    }
    if (reduceInstance) {
        delete reduceInstance;
        FreeLibrary(hReduceDll);
    }
}

void Executive::markSuccess() {
    std::string successFilePath = fileManagement.getOutputDirectory() + "/SUCCESS";
    std::ofstream successFile(successFilePath);
    if (successFile) {
        successFile << "Processing completed successfully." << std::endl;
        successFile.close();
        std::cout << "SUCCESS file created." << std::endl;
    }
    else {
        std::cerr << "Failed to create SUCCESS file." << std::endl;
    }
}

void Executive::loadMapDll(const std::string& path) {
    hMapDll = LoadLibraryA(path.c_str());
    if (!hMapDll) {
        std::cerr << "Failed to load Map DLL" << std::endl;
        return;
    }
    CREATEMAPFUNC createMap = (CREATEMAPFUNC)GetProcAddress(hMapDll, "CreateMapInstance");
    if (!createMap) {
        std::cerr << "Failed to find CreateMapInstance function" << std::endl;
        return;
    }


    // Create multiple instances of the map function
    for (int i = 0; i < numReducers; ++i) {
        IMap* mapInstance = createMap(&fileManagement, bufferSize, numReducers);
        if (mapInstance) {
            mapInstances.push_back(mapInstance);
        }
        else {
            std::cerr << "Failed to create map instance" << std::endl;
        }
    }
}

void Executive::loadReduceDll(const std::string& path) {
    hReduceDll = LoadLibraryA(path.c_str());
    if (!hReduceDll) {
        std::cerr << "Failed to load Reduce DLL" << std::endl;
        return;
    }
    CREATEREDUCEFUNC createReduce = (CREATEREDUCEFUNC)GetProcAddress(hReduceDll, "CreateReduceInstance");
    if (!createReduce) {
        std::cerr << "Failed to find CreateReduceInstance function" << std::endl;
        return;
    }
    reduceInstance = createReduce(&fileManagement);
}
