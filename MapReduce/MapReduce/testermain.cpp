#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include "Executive.h"
#include "C:\Users\mark\source\repos\CIS687_Project3\CSE687_Project3\MapReduce\shared\include\IMap.h"
#include "C:\Users\mark\source\repos\CIS687_Project3\CSE687_Project3\MapReduce\shared\include\IReduce.h"
#include "Sort.h"

namespace fs = std::filesystem;

int main() {

    std::string inputDir1 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\InputDir1";
    std::string inputDir2 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\InputDir2";

    std::string tempDir1 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\TempDir1";
    std::string tempDir2 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\TempDir2";

    std::string outputDir1 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\ReducedDir1";
    std::string outputDir2 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\ReducedDir2";
    std::string outputDir3 = "C:\\Users\\mark\\source\\repos\\CIS687_Project3TestDirs\\FinalDir";

    FileManagement manager1(inputDir1, tempDir1, outputDir1);
    FileManagement manager2(inputDir2, tempDir2, outputDir2);
    FileManagement manager3(inputDir1, tempDir1, outputDir3);
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

    std::string tempFile = "\\temp_output.txt";
    //std::string tempFile2 = "\\temp_output2.txt";
    //std::string tempFile3 = "\\temp_output3.txt";

    IMap* map1 = createMap(manager1, bufferSize, tempFile);
    IMap* map2 = createMap(manager2, bufferSize, tempFile);


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

    std::string inFile = "\\sorted_aggregated_output.txt";
    //std::string inFile2 = "\\sorted_aggregated_output.txt";

    std::string outFile1 = "\\reduced_output1.txt";
    std::string outFile2 = "\\reduced_output2.txt";

    IReduce* reduce1 = createReduce(manager1, inFile, outFile1);
    IReduce* reduce2 = createReduce(manager2, inFile, outFile2);

    Sort sorter1(&manager1);
    Sort sorter2(&manager2);

    sorter1.sortAndAggregate();
    sorter2.sortAndAggregate();

    reduce1->ReduceFunction();
    reduce2->ReduceFunction();
    
    std::string outFile3 = "\\final_output.txt";

    IReduce* reduce3 = createReduce(manager3, inFile, outFile3);

    std::string sourceFile1 = outputDir1 + "\\reduced_output1.txt";
    //sourceFile1.append(outFile1);
    std::string sourceFile2 = outputDir2 + "\\reduced_output2.txt";
    //sourceFile2.append(outFile2);

    reduce3->finalSort(sourceFile1, sourceFile2);

    return 0;

}