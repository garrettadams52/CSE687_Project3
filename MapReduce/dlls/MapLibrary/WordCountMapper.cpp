#include "WordCountMapper.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

// Constructor: Initializes the mapper with a file manager, buffer size, and number of reducers.
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
        outputFiles.push_back(new FileWriter(fullPath)); // Use FileWriter to manage file stream
    }
}

// Destructor: Flushes the buffer and closes all output files.
WordCountMapper::~WordCountMapper() {
    flushBuffer();
    for (auto& file : outputFiles) {
        delete file; // Ensure proper cleanup of file writers
    }
}

// Map function: Processes the content of the input file.
void WordCountMapper::map(const std::string& fileName, const std::string& content) {
    std::istringstream stream(content);
    std::string word;
    while (stream >> word) {
        // Remove non-alphabetical characters and convert to lowercase.
        word.erase(std::remove_if(word.begin(), word.end(),
            [](unsigned char c) { return !std::isalpha(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            exportToFile(word);
        }
    }
}

// Export function: Adds word to buffer and flushes buffer if it exceeds the size.
void WordCountMapper::exportToFile(const std::string& word) {
    int partition = partitionFunction(word); // Determine the partition for the word.
    buffer.push_back({ partition, "(" + word + ", 1)" }); // Add word with partition info to buffer.
    if (buffer.size() >= bufferSize) {
        flushBuffer(); // Flush buffer if it exceeds the buffer size.
    }
}

// Partition function: Determines the partition for a given key using a hash function.
int WordCountMapper::partitionFunction(const std::string& key) {
    std::hash<std::string> hashFn;
    return hashFn(key) % numReducers; // Use hash function to assign partition based on key.
}

// Flush function: Writes buffered entries to the appropriate output files.
void WordCountMapper::flushBuffer() {
    for (const auto& entry : buffer) {
        int partition = entry.first; // Get partition info from buffer entry.
        outputFiles[partition]->write(entry.second); // Write entry to the corresponding output file.
    }
    buffer.clear(); // Clear buffer after flushing.
}

// Factory function: Creates an instance of the WordCountMapper with the given parameters.
extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize, int numReducers) {
    return new WordCountMapper(fileManager, bufferSize, numReducers); // Return a new instance of WordCountMapper.
}
