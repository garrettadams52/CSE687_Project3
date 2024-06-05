#ifndef SUM_REDUCER_H
#define SUM_REDUCER_H

#include "IFileManagement.h"
#include "IReduce.h"
#include <string>
#include <vector>
#include <winsock2.h>

#ifdef REDUCELIBRARY_EXPORTS
#define REDUCELIBRARY_API __declspec(dllexport)
#else
#define REDUCELIBRARY_API __declspec(dllimport)
#endif

class REDUCELIBRARY_API SumReducer : public IReduce {
private:
    IFileManagement* fileManager;
    std::string outputPath;
    std::string inputFileName;
    std::string outputFileName;
    SOCKET sendSocket;

public:
    SumReducer(IFileManagement* fileManager, const std::string& inputFileName, const std::string& outputFileName);
    virtual ~SumReducer();

    virtual void reduce();
    virtual void ReduceFunction() override {
        reduce();
    }

    virtual void finalSort(std::string filePath1, std::string filePath2) override;

private:
    void exportResult(const std::string& key, int result);
    void initSocket();
    void sendHeartbeats();
};

extern "C" REDUCELIBRARY_API IReduce * CreateReduceInstance(IFileManagement * fileManager, const std::string & inputFilePath, const std::string & outputFilePath);

#endif // SUM_REDUCER_H
