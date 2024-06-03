#include "Workflow.h"
#include <iostream>
#include "Sort.h"
#include "FileManagement.h"
#include <vector>
#include <thread>
#include <sstream>
#include <functional>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h> // For _mkdir

Workflow::Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir, IMap* map, IReduce* reduce)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir), mapInstance(map), reduceInstance(reduce), fileManagement(inputDir, tempDir, outputDir) {}

void Workflow::run() {
    std::cout << "Controller starting to run mappers..." << std::endl;

    std::vector<std::thread> mapperThreads;
    int numReducers = 2; 
    int numMappers = numReducers + 1;

    // Vector to copy in all files from the input directory
    std::vector<std::string> files = fileManagement.getAllFiles();

    // Vector to store subset of files from input directory
    std::vector<std::vector<std::string>> fileVec(numMappers);

    // Iterator to keep track of files index position
    int filesIterator = 0;
    int filesPerVec = files.size() / numMappers;

    // Iterates through each element of fileVec, assigning each roughly one third of the files
    // in the input directory.
    for (int i = 0; i < numMappers; ++i) {
        for (int j = 0; j < filesPerVec; ++j) {
            fileVec[i].push_back(files[filesIterator]);
            ++filesIterator;
        }
    }

    // Copies any leftover files into the last element of fileVec (likely to be leftovers due to imprecision of integer division)
    while (filesIterator < files.size()) {
        fileVec[numMappers - 1].push_back(files[filesIterator]);
        ++filesIterator;
    }

    // Initializes path names for subdirectories which will store temp output files. This is necessary because
    // the sort class takes files from two directories, then separately sorts the files from each and places them
    // into one of the two files the reducer will use
    std::string tempDir1 = fileManagement.getTempDirectory() + "\\subDir1";
    std::string tempDir2 = fileManagement.getTempDirectory() + "\\subDir2";

    // Initializes subdirectories
    _mkdir(tempDir1.c_str());
    _mkdir(tempDir2.c_str());

    std::vector<std::string> tempDirVec{ tempDir1, tempDir2 };

    // Launch mapper threads
    for (int i = 0; i < numMappers; ++i) {
        mapperThreads.emplace_back([this, i, numReducers, &fileVec, &tempDirVec]() {
            // Mapper logic
            if (!mapInstance) {
                std::cerr << "Map instance not initialized.\n";
                return;
            }

            // Read all files and process each line with the Map function
            for (const auto& file : fileVec[i]) {
                auto lines = fileManagement.readFile(file);
                for (const auto& line : lines) {
                    mapInstance->MapFunction(file, line);
                }
            }

            std::cout << "Mapper process " << i + 1 << " completed" << std::endl;
            });
    }

    // Wait for all mapper threads to complete
    for (auto& thread : mapperThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "Mapping phase completed. Sorting and aggregating results..." << std::endl;

    Sort sorter(&fileManagement);
    sorter.sortAndAggregate();  // Perform sorting and aggregation

    std::cout << "Starting reduce phase..." << std::endl;

    reduceInstance->ReduceFunction();  // Run the reduce phase
}
