#include <iostream>
#include <string>
#include <thread>
#include <windows.h>
#include <vector>
#include <sstream>
#include <filesystem>
#include "Executive.h"

namespace fs = std::filesystem;

const int BUFFER_SIZE = 1024;
const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\mapreduce_pipe";

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void startMapProcess(const std::vector<std::string>& params) {
    if (params.size() != 6) {
        std::cerr << "Incorrect number of parameters for map process." << std::endl;
        return;
    }

    std::string inputDir = params[1];
    std::string tempDir = params[2];
    std::string outputDir = params[3];
    std::string mapDllPath = params[4];
    std::string reduceDllPath = params[5];
    int bufferSize = 1000;
    int numReducers = 2;

    std::cout << "Starting Map Process with parameters:" << std::endl;
    std::cout << "Input Directory: " << inputDir << std::endl;
    std::cout << "Temp Directory: " << tempDir << std::endl;
    std::cout << "Output Directory: " << outputDir << std::endl;
    std::cout << "Map DLL Path: " << mapDllPath << std::endl;

    Executive exec(inputDir, tempDir, outputDir, mapDllPath, reduceDllPath, bufferSize, numReducers);
    exec.run("mapper", 0);
}

void startReduceProcess(const std::vector<std::string>& params) {
    if (params.size() != 6) {
        std::cerr << "Incorrect number of parameters for reduce process." << std::endl;
        return;
    }

    std::string inputDir = params[1];
    std::string tempDir = params[2];
    std::string outputDir = params[3];
    std::string mapDllPath = params[4];
    std::string reduceDllPath = params[5];
    int bufferSize = 1000;
    int numReducers = 2;

    std::cout << "Starting Reduce Process with parameters:" << std::endl;
    std::cout << "Input Directory: " << inputDir << std::endl;
    std::cout << "Temp Directory: " << tempDir << std::endl;
    std::cout << "Output Directory: " << outputDir << std::endl;
    std::cout << "Reduce DLL Path: " << reduceDllPath << std::endl;

    Executive exec(inputDir, tempDir, outputDir, mapDllPath, reduceDllPath, bufferSize, numReducers);
    exec.run("reducer", 0);
}

void stubProcess() {
    HANDLE pipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe failed, GLE=" << GetLastError() << std::endl;
        return;
    }

    std::cout << "Stub process waiting for connection on named pipe..." << std::endl;

    BOOL connected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!connected) {
        std::cerr << "ConnectNamedPipe failed, GLE=" << GetLastError() << std::endl;
        CloseHandle(pipe);
        return;
    }

    std::cout << "Client connected." << std::endl;

    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(pipe, buffer, BUFFER_SIZE, &bytesRead, NULL);
        if (success && bytesRead != 0) {
            std::string message(buffer, bytesRead);
            std::cout << "Message received: " << message << std::endl;
            auto params = split(message, ' ');

            if (params[0] == "map") {
                std::thread(startMapProcess, params).detach();
            }
            else if (params[0] == "reduce") {
                std::thread(startReduceProcess, params).detach();
            }
        }
        else if (bytesRead == 0) {
            std::cout << "Connection closed by client." << std::endl;
            break;
        }
        else {
            std::cerr << "ReadFile failed, GLE=" << GetLastError() << std::endl;
            break;
        }
    }

    CloseHandle(pipe);
}

int main() {
    stubProcess();
    return 0;
}
