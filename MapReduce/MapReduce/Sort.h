#ifndef SORT_H
#define SORT_H

#include <string>
#include <vector>
#include <map>
#include "FileManagement.h"


class Sort {
public:
    explicit Sort(FileManagement* fileManager);
    void sortAndAggregate();
    int numReducers; // Add this member to keep track of the number of reducers
private:
    FileManagement* fileManager;
    std::map<std::string, std::vector<int>> aggregateData(const std::vector<std::pair<std::string, int>>& data);
};

#endif 
