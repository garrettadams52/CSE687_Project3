#include "WordCountMapper.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

// Constructor: Initializes the mapper with file manager, buffer size, and number of reducers
WordCountMapper::WordCountMapper(IFileManagement* fileManager, size_t bufferSize, int numReducers)
    : fileManager(fileManager), bufferSize(bufferSize), numReducers(numReducers) {
    if (!fileManager) {
        throw std::invalid_argument("fileManager pointer cannot be null.");
    }

    std::string tempDirectory = fileManager->getTempDirectory();
    if (tempDirectory.empty()) {
        throw std::runtime_error("Temporary directory cannot be empty.");
    }

    if (!fs::exists(tempDirectory)) {
        fs::create_directories(tempDirectory);
    }

    // Open output files for each reducer
    for (int i = 0; i < numReducers; ++i) {
        std::string fullPath = tempDirectory + "\\temp_output_" + std::to_string(i) + ".txt";
        outputFiles.push_back(new FileWriter(fullPath));
    }
}

// Destructor: Flushes the buffer and closes all output files
WordCountMapper::~WordCountMapper() {
    flushBuffer();
    for (auto fileWriter : outputFiles) {
        delete fileWriter;
    }
}

// Map function: Processes the content of the input file
void WordCountMapper::map(const std::string& fileName, const std::string& content) {
    std::istringstream stream(content);
    std::string word;
    while (stream >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](unsigned char c) { return !std::isalpha(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            exportToFile(word);
        }
    }
}

// Exports the word to the appropriate output file based on partition
void WordCountMapper::exportToFile(const std::string& word) {
    int partition = partitionFunction(word);
    buffer.emplace_back(partition, "(" + word + ", 1)");
    if (buffer.size() >= bufferSize) {
        flushBuffer();
    }
}

// Flushes the buffer to the output files
void WordCountMapper::flushBuffer() {
    for (const auto& entry : buffer) {
        outputFiles[entry.first]->write(entry.second);
    }
    buffer.clear();
}

// Partition function: Determines the partition for a given key
int WordCountMapper::partitionFunction(const std::string& key) {
    std::hash<std::string> hashFunction;
    return hashFunction(key) % numReducers;
}

// Factory function: Creates an instance of the WordCountMapper
extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize, int numReducers) {
    return new WordCountMapper(fileManager, bufferSize, numReducers);
}
