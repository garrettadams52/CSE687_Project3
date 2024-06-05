#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <windows.h>
#include <thread>
#pragma comment(lib, "kernel32.lib")

namespace fs = std::filesystem;

std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void sendMessage(const std::string& message, const std::wstring& pipeName) {
    HANDLE pipe;
    while (true) {
        pipe = CreateFile(
            pipeName.c_str(),   // pipe name
            GENERIC_WRITE,      // write access
            0,                  // no sharing
            NULL,               // default security attributes
            OPEN_EXISTING,      // opens existing pipe
            0,                  // default attributes
            NULL);              // no template file

        if (pipe != INVALID_HANDLE_VALUE) {
            break;
        }

        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "Failed to connect to pipe. Error: " << GetLastError() << std::endl;
            return;
        }

        // All pipe instances are busy, so wait.
        if (!WaitNamedPipe(pipeName.c_str(), 20000)) {
            std::cerr << "Could not open pipe: 20 second wait timed out." << std::endl;
            return;
        }
    }

    DWORD bytesWritten = 0;
    BOOL success = WriteFile(
        pipe,                      // handle to pipe
        message.c_str(),           // message
        message.length(),          // message length
        &bytesWritten,             // bytes written
        NULL);                     // not overlapped I/O

    if (!success) {
        std::cerr << "Failed to write to pipe. Error: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Message sent: " << message << std::endl;
    }

    CloseHandle(pipe);
}

int main() {
    std::string inputLine;
    std::cout << "Enter the input file path, temporary directory, output directory, Map DLL path, and Reduce DLL path all separated by spaces:\n";
    std::cout << "Example: C:\\Path\\To\\InputFiles C:\\Path\\To\\Temp C:\\Path\\To\\Output C:\\Path\\To\\Map.dll C:\\Path\\To\\Reduce.dll\n";
    std::getline(std::cin, inputLine);

    // Validate and send the message to the stub process
    if (inputLine.empty()) {
        std::cerr << "Error: No input provided." << std::endl;
        return 1;
    }

    std::istringstream iss(inputLine);
    std::vector<std::string> params((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

    if (params.size() != 5) {
        std::cerr << "Error: You must enter exactly five paths separated by spaces." << std::endl;
        return 1;
    }

    if (!fs::exists(params[0]) || !fs::is_directory(params[0]) ||
        !fs::exists(params[3]) || !fs::exists(params[4])) {
        std::cerr << "Error: One or more specified paths do not exist or are not accessible." << std::endl;
        return 1;
    }

    std::wstring pipeName = L"\\\\.\\pipe\\stubPipe";
    sendMessage(inputLine, pipeName);

    return 0;
}
