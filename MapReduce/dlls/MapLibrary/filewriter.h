#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <fstream>
#include <string>

// The FileWriter class manages the opening, writing, and closing of file streams.
// It ensures that files are properly handled, reducing the risk of file handling errors.
class FileWriter {
public:
    // Constructor: Opens the file at the specified file path in append mode.
    // Throws a runtime_error if the file cannot be opened.
    FileWriter(const std::string& filePath) {
        outputFile.open(filePath, std::ios::app);
        if (!outputFile) {
            throw std::runtime_error("Failed to open output file at: " + filePath);
        }
    }

    // Destructor: Closes the file stream if it is open.
    ~FileWriter() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }

    // Write Method: Writes a line of data to the file, followed by a newline.
    void write(const std::string& data) {
        outputFile << data << std::endl;
    }

private:
    std::ofstream outputFile; // Output file stream used for writing data to the file.
};

#endif // FILE_WRITER_H
