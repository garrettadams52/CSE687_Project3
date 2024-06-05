#include "WordCountMapper.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <thread>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace fs = std::filesystem;

WordCountMapper::WordCountMapper(IFileManagement* fileManager, size_t bufferSize)
    : fileManager(fileManager), bufferSize(bufferSize) {
    if (!fileManager) {
        throw std::invalid_argument("fileManager pointer cannot be null.");
    }

    std::string tempDirectory = fileManager->getTempDirectory();
    if (tempDirectory.empty()) {
        throw std::runtime_error("Temporary directory cannot be empty.");
    }

    if (!fs::exists(tempDirectory)) {
        fs::create_directories(tempDirectory);
    }

    initSocket();
}

WordCountMapper::~WordCountMapper() {
    flushBuffer();
    if (outputFile.is_open()) {
        outputFile.close();
    }
    closesocket(sendSocket);
    WSACleanup();
}

void WordCountMapper::initSocket() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return;
    }

    struct addrinfo* addrResult = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo("127.0.0.1", "27015", &hints, &addrResult);
    if (result != 0) {
        std::cerr << "getaddrinfo failed with error: " << result << std::endl;
        WSACleanup();
        return;
    }

    sendSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (sendSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return;
    }

    result = connect(sendSocket, addrResult->ai_addr, static_cast<int>(addrResult->ai_addrlen));
    if (result == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(sendSocket);
        sendSocket = INVALID_SOCKET;
    }

    freeaddrinfo(addrResult);

    if (sendSocket == INVALID_SOCKET) {
        std::cerr << "Unable to connect to server!" << std::endl;
        WSACleanup();
        return;
    }

    std::thread(&WordCountMapper::sendHeartbeats, this).detach();
}

void WordCountMapper::sendHeartbeats() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::string message = "Map process heartbeat.";
        int result = send(sendSocket, message.c_str(), static_cast<int>(message.size()), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

void WordCountMapper::map(const std::string& fileName, const std::string& content) {
    std::istringstream stream(content);
    std::string word;
    while (stream >> word) {
        word.erase(std::remove_if(word.begin(), word.end(),
            [](unsigned char c) { return !std::isalpha(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) {
            exportToFile(word);
        }
    }
}

void WordCountMapper::exportToFile(const std::string& word) {
    buffer.push_back("(" + word + ", 1)");
    if (buffer.size() >= bufferSize) {
        flushBuffer();
    }
}

void WordCountMapper::flushBuffer() {
    for (const auto& entry : buffer) {
        outputFile << entry << std::endl;
    }
    buffer.clear();
}

extern "C" MAPLIBRARY_API IMap * CreateMapInstance(IFileManagement * fileManager, size_t bufferSize) {
    return new WordCountMapper(fileManager, bufferSize);
}
