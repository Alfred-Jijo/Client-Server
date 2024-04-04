#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

std::mutex clientsMutex;
std::vector<SOCKET> clients;
std::atomic_bool isRunning(true);

void handleClient(SOCKET clientSocket)
{
    char buffer[1024];
    int bytesReceived;
    do
    {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0)
        {
            std::string message(buffer, bytesReceived);
            std::cout << "Received message: " << message << std::endl;
        }
        else
        {
            // Client disconnected
            break;
        }
    } while (bytesReceived > 0);

    // Remove disconnected client from the list
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());

    closesocket(clientSocket);
}

int main()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating server socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Enter the port to listen on: ";
    unsigned short port;
    std::cin >> port;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    std::vector<std::thread> clientThreads;

    while (isRunning)
    {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(clientSocket);

        clientThreads.emplace_back(handleClient, clientSocket);
    }

    // Gracefully shut down existing connections
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (SOCKET client : clients)
    {
        closesocket(client);
    }

    for (auto &thread : clientThreads)
    {
        thread.join();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}