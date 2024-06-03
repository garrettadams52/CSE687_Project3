#include "Executive.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

// Helper function for char* to LPWSTR conversion
std::wstring convertCharToLPWSTR(const char* charArray) {
    std::wstring wideString = std::wstring(charArray, charArray + strlen(charArray));
    return wideString;
}

// Constructor implementation
Executive::Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
    const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize, int reducers)
    : fileManagement(inputDir, tempDir, outputDir),
    hMapDll(nullptr), hReduceDll(nullptr), 
    mapInstance(nullptr), reduceInstance(nullptr),
    bufferSize(bufSize), numReducers(reducers),
    workflow(inputDir, tempDir, outputDir, reduceDllPath, nullptr, nullptr) {  // Temporarily initialize with nullptr

    loadMapDll(mapDllPath);
    loadReduceDll(reduceDllPath);

    if (mapInstance && reduceInstance) {
        workflow = Workflow(inputDir, tempDir, outputDir, reduceDllPath, mapInstance, reduceInstance);
    }
    else {
        std::cerr << "Failed to load map or reduce DLLs correctly.\n";
    }
}

void Executive::run(const std::string& mode, int mapperIndex) {
    if (mapInstance && reduceInstance) {
        workflow.run();
    }
    else {
        std::cerr << "Map or Reduce instances are not initialized.\n";
    }
    markSuccess();
}

// Destructor implementation
Executive::~Executive() {
    if (mapInstance) {
        delete mapInstance;
        FreeLibrary(hMapDll);
    }
    if (reduceInstance) {
        delete reduceInstance;
        FreeLibrary(hReduceDll);
    }
}

// Mark success method implementation
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

// Load map DLL implementation
void Executive::loadMapDll(const std::string& path) {
    hMapDll = LoadLibraryW(convertCharToLPWSTR(path.c_str()).c_str());
    if (!hMapDll) {
        std::cerr << "Failed to load Map DLL" << std::endl;
        return;
    }
    CREATEMAPFUNC createMap = (CREATEMAPFUNC)GetProcAddress(hMapDll, "CreateMapInstance");
    if (!createMap) {
        std::cerr << "Failed to find CreateMapInstance function" << std::endl;
        return;
    }
    mapInstance = createMap(fileManagement, bufferSize, numReducers, 0); // Adjust the parameters as needed
}

// Load reduce DLL implementation
void Executive::loadReduceDll(const std::string& path) {
    hReduceDll = LoadLibraryW(convertCharToLPWSTR(path.c_str()).c_str());
    if (!hReduceDll) {
        std::cerr << "Failed to load Reduce DLL" << std::endl;
        return;
    }
    CREATEREDUCEFUNC createReduce = (CREATEREDUCEFUNC)GetProcAddress(hReduceDll, "CreateReduceInstance");
    if (!createReduce) {
        std::cerr << "Failed to find CreateReduceInstance function" << std::endl;
        return;
    }

    std::string inFile = "";
    std::string outFile = "\\final_output.txt";

    reduceInstance = createReduce(fileManagement, inFile, outFile);
}
