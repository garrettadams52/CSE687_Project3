#include "Workflow.h"
#include <iostream>
#include "Sort.h"
#include "FileManagement.h"
#include "IMap.h"
#include "IReduce.h"
#include <thread>
#include <vector>
#include <sstream>
#include <fstream> // Include for std::ofstream
#include <functional> // Include for std::hash

// Partition function to determine which reducer should handle a given key
int Workflow::partitionFunction(const std::string& key, int numReducers) {
    std::hash<std::string> hashFn;
    return hashFn(key) % numReducers;
}

Workflow::Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir, IMap* map, IReduce* reduce)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir), mapInstance(map), reduceInstance(reduce), fileManagement(inputDir, tempDir, outputDir) {}

void Workflow::run() {
    std::cout << "Controller starting to run mappers..." << std::endl;

    std::vector<std::thread> mapperThreads;
    int numReducers = 2; // Or any other appropriate value

    // Launch mapper threads
    for (int i = 0; i < numReducers + 1; ++i) {
        mapperThreads.emplace_back([this, i, numReducers]() {
            // Mapper logic
            if (!mapInstance) {
                std::cerr << "Map instance not initialized.\n";
                return;
            }

            // Read all files and process each line with the Map function
            std::vector<std::string> files = fileManagement.getAllFiles();
            for (const auto& file : files) {
                auto lines = fileManagement.readFile(file);
                for (const auto& line : lines) {
                    std::istringstream stream(line);
                    std::string word;
                    while (stream >> word) {
                        int reducerIndex = Workflow::partitionFunction(word, numReducers);
                        std::string outputFileName = fileManagement.getTempDirectory() + "\\mapper_" + std::to_string(i) + "_reducer_" + std::to_string(reducerIndex) + ".txt";
                        std::ofstream outputFile(outputFileName, std::ios::app);
                        if (outputFile.is_open()) {
                            outputFile << "(" << word << ", 1)" << std::endl;
                        }
                        else {
                            std::cerr << "Error opening file: " << outputFileName << std::endl;
                        }
                    }
                }
            }

            std::cout << "Mapper process " << i << " completed" << std::endl;
            });
    }

    // Wait for all mapper threads to complete
    for (auto& thread : mapperThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "Mappers completed. Sorting and aggregating results..." << std::endl;

    Sort sorter(&fileManagement);
    sorter.sortAndAggregate();  // Perform sorting and aggregation

    std::cout << "Starting reduce phase..." << std::endl;
    reduceInstance->ReduceFunction();  // Run the reduce phase
}
