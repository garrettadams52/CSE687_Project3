#include "Executive.h"
#include <iostream>
#include <fstream>
#include <thread>

Executive::Executive(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir,
    const std::string& mapDllPath, const std::string& reduceDllPath, int bufSize, int numReducers)
    : fileManagement(inputDir, tempDir, outputDir),
    bufferSize(bufSize), numReducers(numReducers), reduceInstance(nullptr), workflow(nullptr) {

    loadMapDlls(mapDllPath);
    loadReduceDll(reduceDllPath);

    if (!mapInstances.empty() && reduceInstance) {
        workflow = new Workflow(inputDir, tempDir, outputDir, mapInstances, reduceInstance);
    }
    else {
        std::cerr << "Failed to load map or reduce DLLs correctly.\n";
    }
}

Executive::~Executive() {
    for (auto mapInstance : mapInstances) {
        if (mapInstance) {
            delete mapInstance;
        }
    }
    if (reduceInstance) {
        delete reduceInstance;
    }
    for (auto hMapDll : hMapDlls) {
        FreeLibrary(hMapDll);
    }
    if (hReduceDll) {
        FreeLibrary(hReduceDll);
    }
    if (workflow) {
        delete workflow;
    }
}

void Executive::loadMapDlls(const std::string& path) {
    for (int i = 0; i < numReducers + 1; ++i) {
        HMODULE hMapDll = LoadLibraryA(path.c_str());
        if (!hMapDll) {
            std::cerr << "Failed to load Map DLL" << std::endl;
            return;
        }
        CREATEMAPFUNC createMap = (CREATEMAPFUNC)GetProcAddress(hMapDll, "CreateMapInstance");
        if (!createMap) {
            std::cerr << "Failed to find CreateMapInstance function" << std::endl;
            return;
        }
        IMap* mapInstance = createMap(&fileManagement, bufferSize, numReducers);
        if (mapInstance) {
            mapInstances.push_back(mapInstance);
            hMapDlls.push_back(hMapDll);
        }
        else {
            std::cerr << "Failed to create map instance " << i << std::endl;
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
    if (!reduceInstance) {
        std::cerr << "Failed to create reduce instance" << std::endl;
    }
}

void Executive::run() {
    if (workflow) {
        std::vector<std::thread> threads;
        for (int i = 0; i < numReducers + 1; ++i) {
            threads.emplace_back([this, i]() {
                this->workflow->runMap(i);
                });
        }
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        workflow->sortAndAggregate();
        workflow->runReduce();
    }
    else {
        std::cerr << "Workflow is not initialized.\n";
    }
    markSuccess();
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
