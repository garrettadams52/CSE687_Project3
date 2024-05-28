#ifndef I_MAP_H
#define I_MAP_H

#include <string>

#ifdef MAPLIBRARY_EXPORTS
#define MAPLIBRARY_API __declspec(dllexport)
#else
#define MAPLIBRARY_API __declspec(dllimport)
#endif

// Interface for the Map function in the MapReduce framework
class MAPLIBRARY_API IMap {
public:
    // Pure virtual function that must be implemented by any class that inherits from IMap
    // This function will process the content of a file specified by filename
    virtual void MapFunction(const std::string& filename, const std::string& content) = 0;

    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~IMap() {}
};

#endif // I_MAP_H
