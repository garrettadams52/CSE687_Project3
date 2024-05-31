#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "Executive.h"
#include "C:\Users\mark\source\repos\CIS687_Project3\CSE687_Project3\MapReduce\shared\include\IMap.h"
#include "C:\Users\mark\source\repos\CIS687_Project3\CSE687_Project3\MapReduce\shared\include\IReduce.h"
#include "Sort.h"

namespace fs = std::filesystem;

int main() {

    std::string inputDir1 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\InputDir1";
    std::string inputDir2 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\InputDir2";
    std::string inputDir3 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\InputDir3";
    std::string inputDir4 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\InputDir4";

    std::string tempDir1 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\TempDir1";
    std::string tempDir2 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\TempDir2";

    std::string outputDir1 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\ReducedDir1";
    std::string outputDir2 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\ReducedDir2";
    std::string outputDir3 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\FinalDir";

    FileManagement manager1(inputDir1, tempDir1, outputDir1);
    FileManagement manager2(inputDir2, tempDir2, outputDir2);
    FileManagement manager3(inputDir3, tempDir1, outputDir1);
    FileManagement manager4(inputDir4, tempDir2, outputDir1);
    FileManagement manager5(inputDir1, tempDir1, outputDir3);

    int bufferSize = 1000;

   HMODULE hMapDll = LoadLibrary(L"C:\\Users\\mark\\source\\repos\\CIS687_Project3\\CSE687_Project3\\MapReduce\\x64\\Debug\\MapLibrary.dll");
    if (!hMapDll) {
        std::cerr << "Failed to load Map DLL" << std::endl;
        //return -1;
    }

    HMODULE hReduceDll = LoadLibrary(L"C:\\Users\\mark\\source\\repos\\CIS687_Project3\\CSE687_Project3\\MapReduce\\x64\\Debug\\ReduceLibrary.dll");
    if (!hReduceDll) {
        std::cerr << "Failed to load Reduce DLL" << std::endl;
        //return -1;
    }

    CREATEMAPFUNC createMap = (CREATEMAPFUNC)GetProcAddress(hMapDll, "CreateMapInstance");
    if (!createMap) {
        std::cerr << "Failed to find CreateMapInstance function" << std::endl;
        //return -1;
    }

    CREATEREDUCEFUNC createReduce = (CREATEREDUCEFUNC)GetProcAddress(hReduceDll, "CreateReduceInstance");
    if (!createReduce) {
        std::cerr << "Failed to find CreateReduceInstance function" << std::endl;
        //return -1;
    }

    std::string tempFile1 = "\\temp_output.txt";
    std::string tempFile2 = "\\temp_output2.txt";

    IMap* map1 = createMap(manager1, bufferSize, tempFile1);
    IMap* map2 = createMap(manager2, bufferSize, tempFile2);
    //IMap* map3 = createMap(manager3, bufferSize, tempFile2);
    //IMap* map4 = createMap(manager4, bufferSize, tempFile2);


    auto files1 = manager1.getAllFiles();
    if (files1.empty()) {
        std::cerr << "No files found in the input directory: " << inputDir1 << std::endl;
        //return -1;
    }

    for (const auto& file : files1) {
        auto lines = manager1.readFile(file);
        for (const auto& line : lines) {
            map1->MapFunction(file, line);
        }
    }

    auto files2 = manager2.getAllFiles();
    if (files2.empty()) {
        std::cerr << "No files found in the input directory: " << inputDir2 << std::endl;
        //return -1;
    }

    for (const auto& file : files2) {
        auto lines = manager2.readFile(file);
        for (const auto& line : lines) {
            map2->MapFunction(file, line);
        }
    }

   /* auto files3 = manager3.getAllFiles();
    if (files3.empty()) {
        std::cerr << "No files found in the input directory: " << inputDir2 << std::endl;
        //return -1;
    }

    for (const auto& file : files3) {
        auto lines = manager3.readFile(file);
        for (const auto& line : lines) {
            map3->MapFunction(file, line);
        }
    }

    auto files4 = manager4.getAllFiles();
    if (files4.empty()) {
        std::cerr << "No files found in the input directory: " << inputDir2 << std::endl;
        //return -1;
    }

    for (const auto& file : files4) {
        auto lines = manager4.readFile(file);
        for (const auto& line : lines) {
            map4->MapFunction(file, line);
        }
    }*/

    std::string inFile1 = "\\sorted_aggregated_output1.txt";
    std::string inFile2 = "\\sorted_aggregated_output2.txt";

    std::string outFile1 = "\\reduced_output1.txt";
    std::string outFile2 = "\\reduced_output2.txt";

    IReduce* reduce1 = createReduce(manager1, inFile1, outFile1);
    IReduce* reduce2 = createReduce(manager1, inFile2, outFile2);

    std::string sortDir1 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\TempDir1";
    std::string sortDir2 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\TempDir2";


    Sort sorter(&manager1, sortDir1, sortDir2);
    //Sort sorter2(&manager2);

    sorter.sortAndAggregate();
    //sorter2.sortAndAggregate();

    reduce1->ReduceFunction();
    reduce2->ReduceFunction();
    
    std::string outFile3 = "\\final_output.txt";

    IReduce* reduce3 = createReduce(manager5, inFile1, outFile3);

    std::string sourceFile1 = outputDir1 + "\\reduced_output1.txt";
    //sourceFile1.append(outFile1);
    std::string sourceFile2 = outputDir1 + "\\reduced_output2.txt";
    //sourceFile2.append(outFile2);

    reduce3->finalSort(sourceFile1, sourceFile2);

    return 0;

}