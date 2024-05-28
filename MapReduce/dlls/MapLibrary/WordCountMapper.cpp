#include "WordCountMapper.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

// Merged Constructor
WordCountMapper::WordCountMapper(IFileManagement* fileManager, size_t bufferSize, int numReducers, int mapperID)
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

    // Open output files for each reducer with unique file names including mapperID
    for (int i = 0; i < numReducers; ++i) {
        std::string fullPath = tempDirectory + "\\m" + std::to_string(mapperID) + std::to_string(i) + ".txt";
        std::ofstream* outFile = new std::ofstream(fullPath);
        if (!outFile->is_open()) {
            delete outFile;
            throw std::runtime_error("Failed to open file: " + fullPath);
        }
        outputFiles.push_back(outFile);
    }
}

WordCountMapper::~WordCountMapper() {
    flushBuffer();
    for (auto fileWriter : outputFiles) {
        if (fileWriter->is_open()) {
            fileWriter->close();
        }
        delete fileWriter;
    }
}

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

void WordCountMapper::exportToFile(const std::string& word) {
    int partition = partitionFunction(word);
    buffer.emplace_back(partition, "(" + word + ", 1)");
    if (buffer.size() >= bufferSize) {
        flushBuffer();
    }
}

void WordCountMapper::flushBuffer() {
    for (const auto& entry : buffer) {
        *(outputFiles[entry.first]) << entry.second << std::endl;
    }
    buffer.clear();
}

int WordCountMapper::partitionFunction(const std::string& key) {
    std::hash<std::string> hashFunction;
    return hashFunction(key) % numReducers;
}

// Factory function with merged logic
extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize, int numReducers, int mapperID) {
    return new WordCountMapper(fileManager, bufferSize, numReducers, mapperID);
}
