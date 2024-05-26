#ifndef SORT_H
#define SORT_H

#include "FileManagement.h"
#include <vector>
#include <map>
#include <string>

class Sort {
    FileManagement* fileManager;
    int numReducers;

public:
    Sort(FileManagement* fileManager, int numReducers); // Ensure constructor matches
    void sortAndAggregate();

private:
    std::map<std::string, std::vector<int>> aggregateData(const std::vector<std::pair<std::string, int>>& data);
};

#endif // SORT_H
