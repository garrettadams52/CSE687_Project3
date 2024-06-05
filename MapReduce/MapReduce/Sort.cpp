#include "Sort.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <iostream>

// Constructor
Sort::Sort(FileManagement* fileManager, std::string inputDir1, std::string inputDir2)
    : fileManager(fileManager), inputDir1(inputDir1), inputDir2(inputDir2) {}

void Sort::sortAndAggregate() {
    std::vector<std::string> dirVec{ inputDir1, inputDir2 };

    int counter = 0;

    for (const std::string& dir : dirVec) {
        ++counter;
        std::vector<std::pair<std::string, int>> entries;
        std::string outputPath = fileManager->getTempDirectory() + "\\sorted_aggregated_output" + std::to_string(counter) + ".txt";
        fileManager->setInputDirectory(dir);
        auto files = fileManager->getAllFiles();
        for (const auto& file : files) {
            auto lines = fileManager->readFile(file);
            for (const std::string& line : lines) {
                std::string word;
                int count;
                std::stringstream ss(line);
                ss.ignore(1, '(');
                getline(ss, word, ',');
                ss.ignore(2, ' ');
                ss >> count;
                ss.ignore(2, ')');
                entries.emplace_back(word, count);
            }
        }

        std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
            });

        std::map<std::string, std::vector<int>> aggregated = aggregateData(entries);

        std::stringstream outputContent;
        for (const auto& [word, counts] : aggregated) {
            outputContent << "(" << word << ", [";
            for (size_t i = 0; i < counts.size(); ++i) {
                outputContent << counts[i];
                if (i < counts.size() - 1) outputContent << ", ";
            }
            outputContent << "])\n";
        }

        fileManager->writeFile(outputPath, outputContent.str(), false);
    }
}

std::map<std::string, std::vector<int>> Sort::aggregateData(const std::vector<std::pair<std::string, int>>& data) {
    std::map<std::string, std::vector<int>> aggregated;
    for (const auto& entry : data) {
        aggregated[entry.first].push_back(entry.second);
    }
    return aggregated;
}
