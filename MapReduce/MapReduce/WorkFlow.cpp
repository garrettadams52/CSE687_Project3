#include "Workflow.h"
#include <iostream>
#include "Sort.h"
#include "FileManagement.h"
#include "IMap.h"
#include "IReduce.h"

Workflow::Workflow(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir, const std::vector<IMap*>& mappers, IReduce* reduce)
    : inputDirectory(inputDir), tempDirectory(tempDir), outputDirectory(outputDir), mapInstances(mappers), reduceInstance(reduce), fileManagement(inputDir, tempDir, outputDir) {}

void Workflow::run() {
    // Clear existing temporary and output files
    std::vector<std::string> filesToClear = { "temp_output.txt", "final_output.txt", "sorted_aggregated_output.txt" };
    fileManagement.clearFiles(tempDirectory, filesToClear);

    // Sorter instance for intermediate sorting and aggregation
    Sort sorter(&fileManagement);

    // Retrieve all input files from the directory
    auto files = fileManagement.getAllFiles();
    if (files.empty()) {
        std::cerr << "No files found in the input directory: " << inputDirectory << std::endl;
        return;
    }

    // Distribute input lines among mappers in a round-robin fashion
    size_t mapperCount = mapInstances.size();
    size_t fileIndex = 0;
    for (const auto& file : files) {
        auto lines = fileManagement.readFile(file);
        for (const auto& line : lines) {
            // Log which mapper is used
            std::cout << "Using mapper instance " << fileIndex % mapperCount << " for line: " << line << std::endl;
            mapInstances[fileIndex % mapperCount]->MapFunction(file, line);
            fileIndex++;
        }
    }

    // Sort and aggregate intermediate results
    sorter.sortAndAggregate();

    // Execute the reduce phase
    reduceInstance->ReduceFunction();
}
