#include "Sort.h"
#include <fstream>
#include <sstream>
#include <algorithm>

Sort::Sort(FileManagement* fileManager)
    : fileManager(fileManager) {}

void Sort::sortAndAggregate() {
    std::string inputPath = fileManager->getTempDirectory() + "\\temp_output.txt";
    std::string outputPath = fileManager->getTempDirectory() + "\\sorted_aggregated_output.txt";

    std::vector<std::pair<std::string, int>> entries;
    std::vector<std::string> lines = fileManager->readFile(inputPath);

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

std::map<std::string, std::vector<int>> Sort::aggregateData(const std::vector<std::pair<std::string, int>>& data) {
    std::map<std::string, std::vector<int>> aggregated;
    for (const auto& entry : data) {
        aggregated[entry.first].push_back(entry.second);
    }
    return aggregated;
}