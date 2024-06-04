#include "Workflow.h"
#include <iostream>
#include "Sort.h"
#include "FileManagement.h"
#include "IMap.h"
#include "IReduce.h"
#include "Executive.h"
#include <exception>
#include <thread>
#include <vector>
#include <sstream>
#include <fstream> // Include for std::ofstream
#include <functional> // Include for std::hash
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>//This directory, and the two above, are needed for _mkdir

// Partition function to determine which reducer should handle a given key
int Workflow::partitionFunction(const std::string& key, int numReducers) {
    std::hash<std::string> hashFn;
    return hashFn(key) % numReducers;
}

Workflow::Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir, 
    const std::string&reduceDllPath, IMap* map, IReduce* reduce)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir), 
    reduceDllPath(reduceDllPath), mapInstance(map), reduceInstance(reduce), fileManagement(inputDir, tempDir, outputDir) {}

void Workflow::run() {
    std::cout << "Controller starting to run mappers..." << std::endl;

    std::vector<std::thread> mapperThreads;
    int numReducers = 2; // Or any other appropriate value
    int numMappers = numReducers + 1;

    //Vector to copy in all files from the input directory
    std::vector<std::string> files = fileManagement.getAllFiles();

    //Vector to store subset of files from input directory
    std::vector<std::vector<std::string>> fileVec(numMappers);

    //Iterator to keep track of files index position
    int filesIterator = 0;
    int filesPerVec = files.size() / numMappers;

    //Iterates through each element of fileVec, assigning each roughly one third of the files
    //in the input directory.
    for (int i = 0; i < numMappers; ++i) {
        for (int j = 0; j < filesPerVec; ++j) {
            fileVec[i].push_back(files[filesIterator]);
            ++filesIterator;
        }
    }

    //Copies any leftover files into the last element of fileVec (likely to be leftovers due to imprecision of integer division)
    //
    //Although numReducers is the same as numMappers - 1, the latter phrase renders the logic easier to follow
    while (filesIterator < files.size()) {
        fileVec[numMappers - 1].push_back(files[filesIterator]);
        ++filesIterator;
    }

    //Initializes path names for subdirectories which will store temp output files. This is necessary because
    //the sort class takes files from two directories, then separately sorts the files from each and places them
    //into one of the two files the reducer will use
    std::string tempDir1 = fileManagement.getTempDirectory() + "\\subDir1";
    std::string tempDir2 = fileManagement.getTempDirectory() + "\\subDir2";

    //Initializes subdirectories
    _mkdir(tempDir1.c_str());
    _mkdir(tempDir2.c_str());

    std::vector<std::string>tempDirVec{ tempDir1, tempDir2 };

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
                    std::istringstream stream(line);
                    std::string word;
                    while (stream >> word) {
                        word.erase(std::remove_if(word.begin(), word.end(),
                            [](unsigned char c) { return !std::isalpha(c); }), word.end());
                        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                        int reducerIndex = Workflow::partitionFunction(word, numReducers);
                        std::string outputFileName = tempDirVec[reducerIndex] + "\\mapper_" + std::to_string(i) + "_reducer_" + std::to_string(reducerIndex) + ".txt";
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

    Sort sorter(&fileManagement, tempDir1, tempDir2);
    sorter.sortAndAggregate();  // Perform sorting and aggregation

    std::cout << "Sorting phase completed. Starting reduce phase..." << std::endl;

    std::string inFile1 = "\\sorted_aggregated_output1.txt";
    std::string inFile2 = "\\sorted_aggregated_output2.txt";

    std::string outFile1 = "\\reduced_output1.txt";
    std::string outFile2 = "\\reduced_output2.txt";

    HMODULE hReduceDll = LoadLibraryA(reduceDllPath.c_str());
    if (!hReduceDll) {
        std::cerr << "Failed to load Reduce DLL" << std::endl;
        return;
    }

    CREATEREDUCEFUNC createReduce = (CREATEREDUCEFUNC)GetProcAddress(hReduceDll, "CreateReduceInstance");
    if (!createReduce) {
        std::cerr << "Failed to find CreateReduceInstance function" << std::endl;
        return;
    }

    IReduce* reduce1 = createReduce(fileManagement, inFile1, outFile1);
    IReduce* reduce2 = createReduce(fileManagement, inFile2, outFile2);

    std::vector<std::thread> reducerThreads;

    //Call reducers and thread them
    reducerThreads.emplace_back([&reduce1]() {
        reduce1->ReduceFunction();
        std::cout << "Reducer process 1 completed\n";
        });
    reducerThreads.emplace_back([&reduce2]() {
        reduce2->ReduceFunction();
        std::cout << "Reducer process 2 completed\n";
        });

    // Wait for all reducer threads to complete
    for (auto& thread : reducerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "Merging final results...\n";

    //Define file path parameters to pass to finalSort
    std::string reduceFile1 = fileManagement.getOutputDirectory() + "\\reduced_output1.txt";
    std::string reduceFile2 = fileManagement.getOutputDirectory() + "\\reduced_output2.txt";

    //Sort the reduced outputs in to one larger array
    reduceInstance->finalSort(reduceFile1, reduceFile2);
}
