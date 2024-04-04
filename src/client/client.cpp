#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating client socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::cout << "Enter the port to connect to: ";
    unsigned short port;
    std::cin >> port;
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::string message;
    std::cout << "Enter a message to send (or 'exit' to quit): ";
    std::getline(std::cin, message);

    while (message != "exit")
    {
        int bytesSent = send(clientSocket, message.c_str(), message.length(), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Enter a message to send (or 'exit' to quit): ";
        std::getline(std::cin, message);
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}