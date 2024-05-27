#include "WordCountMapper.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

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

    for (int i = 0; i < numReducers; ++i) {
        std::string fullPath = tempDirectory + "\\temp_output_" + std::to_string(i) + ".txt";
        std::ofstream* outputFile = new std::ofstream(fullPath, std::ios::app);
        if (!outputFile->is_open()) {
            throw std::runtime_error("Failed to open output file at: " + fullPath);
        }
        outputFiles.push_back(outputFile);
        std::cout << "Opened file: " << fullPath << std::endl; // Log file opening
    }
}

WordCountMapper::~WordCountMapper() {
    flushBuffer();
    for (auto file : outputFiles) {
        if (file->is_open()) {
            file->close();
        }
        delete file;
    }
    std::cout << "Closed all output files" << std::endl; // Log file closure
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
    std::cout << "Processed file: " << fileName << std::endl; // Log file processing
}

void WordCountMapper::exportToFile(const std::string& word) {
    int partition = partitionFunction(word);
    buffer.emplace_back(partition, "(" + word + ", 1)");
    if (buffer.size() >= bufferSize) {
        flushBuffer();
    }
    std::cout << "Exported word: " << word << " to partition: " << partition << std::endl; // Log word export
}

void WordCountMapper::flushBuffer() {
    for (const auto& entry : buffer) {
        *outputFiles[entry.first] << entry.second << std::endl;
    }
    buffer.clear();
    std::cout << "Flushed buffer" << std::endl; // Log buffer flush
}

int WordCountMapper::partitionFunction(const std::string& key) {
    std::hash<std::string> hashFunction;
    int partition = hashFunction(key) % numReducers;
    std::cout << "Partitioned key: " << key << " to partition: " << partition << std::endl; // Log partition function
    return partition;
}

extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize, int numReducers) {
    return new WordCountMapper(fileManager, bufferSize, numReducers);
}
