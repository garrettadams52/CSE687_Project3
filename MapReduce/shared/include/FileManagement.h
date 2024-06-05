#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include "IFileManagement.h"
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

class FileManagement : public IFileManagement {
public:
    FileManagement(const std::string& inputDir, const std::string& tempDir, const std::string& outputDir);


    virtual std::vector<std::string> getAllFiles() const override;
    virtual std::vector<std::string> readFile(const std::string& filePath) const override;
    virtual void clearFiles(const std::string& dirPath, const std::vector<std::string>& fileNames) override;
    virtual void writeFile(const std::string& filePath, const std::string& content, bool append = true) override;
    virtual void createEmptyFile(const std::string& filePath) override;
    virtual std::string getTempDirectory() const override;
    virtual std::string getOutputDirectory() const override;
    virtual void setInputDirectory(std::string inputDirectory) override;

private:
    std::string inputDirectory;
    std::string tempDirectory;
    std::string outputDirectory;
};

#endif // FILEMANAGEMENT_H