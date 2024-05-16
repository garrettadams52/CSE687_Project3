#ifndef SUM_REDUCER_H
#define SUM_REDUCER_H

#include "IFileManagement.h"
#include "IReduce.h"
#include <string>
#include <vector>

#ifdef REDUCELIBRARY_EXPORTS  
#define REDUCELIBRARY_API __declspec(dllexport)
#else
#define REDUCELIBRARY_API __declspec(dllimport)
#endif

class REDUCELIBRARY_API SumReducer : public IReduce {
private:
    IFileManagement* fileManager;
    std::string outputPath;

public:
    SumReducer(IFileManagement* fileManager);
    virtual ~SumReducer() {}

    virtual void reduce();

    virtual void ReduceFunction() override {
        reduce();  
    }

private:
    void exportResult(const std::string& key, int result);
};

#endif // SUM_REDUCER_H

