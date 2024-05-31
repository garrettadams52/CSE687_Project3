#include "Workflow.h"
#include <iostream>
#include "Sort.h"
#include "FileManagement.h"
#include "IMap.h"
#include "IReduce.h"

Workflow::Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir, IMap* map, IReduce* reduce)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir), mapInstance(map), reduceInstance(reduce), fileManagement(inputDir, tempDir, outputDir) {}


void Workflow::run() {

    std::vector<std::string> filesToClear = { "temp_output.txt", "final_output.txt", "sorted_aggregated_output.txt" };
    fileManagement.clearFiles(tempDirectory, filesToClear);

    std::string nonFile1 = "whatever.txt";
    std::string nonFile2 = "whatever.txt";

    Sort sorter(&fileManagement, nonFile1, nonFile2);  

    auto files = fileManagement.getAllFiles();
    if (files.empty()) {
        std::cerr << "No files found in the input directory: " << inputDirectory << std::endl;
        return;
    }

    for (const auto& file : files) {
        auto lines = fileManagement.readFile(file);
        for (const auto& line : lines) {
            mapInstance->MapFunction(file, line);  
        }
    }

    sorter.sortAndAggregate();  

    reduceInstance->ReduceFunction();  
}
