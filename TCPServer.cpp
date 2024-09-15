#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <windows.h>
#include <threads.h>
#include <vector>

using namespace std;
#define BUFFER_SIZE 1024

#pragma comment(lib, "Ws2_32.lib")

void SendFile(SOCKET acceptSocket) {
	Sleep(35000);

	printf("Sending File\n");

	string fileName = "CancelledChequeSend.png";
	string filePath = "C:/Users/knarwani/PracticeProjects/Send File/" + fileName;
	ifstream file(filePath, std::ios::binary);

	if (!file.is_open()) {
		printf("Failed to open file : %s\n", fileName.c_str());
		closesocket(acceptSocket);
		return;
	}

	char sendBuffer[BUFFER_SIZE];
	while (file.read(sendBuffer, sizeof(sendBuffer))) {
		int sendBytes = send(acceptSocket, sendBuffer, file.gcount(), 0);
		if (sendBytes < 0)
		{
			printf("Send failed with error code : %d\n", WSAGetLastError());
			closesocket(acceptSocket);
			return;
		}
	}
	if (file.gcount() > 0) {
		int sendBytes = send(acceptSocket, sendBuffer, file.gcount(), 0);
		if (sendBytes < 0)
		{
			printf("Send failed with error code : %d\n", WSAGetLastError());
			closesocket(acceptSocket);
			return;
		}
	}
	file.close();
	printf("Data sent successfully\n");

	closesocket(acceptSocket);
}

int main()
{

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);

	int portNumber = 8888;

	if (WSAStartup(wVersionRequested, &wsaData)) {
		std::cout << "WSAStartup failed" << std::endl;
		return 0;
	}
	cout << "WSAStartup success" << endl;
	cout << "Status : " << wsaData.szSystemStatus << endl;
	cout << "Status : " << wsaData.szDescription << endl;

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Create socket failed with error code : " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	std::cout << "Create socket success" << std::endl;

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portNumber);
	InetPtonA(AF_INET, PCSTR("127.0.0.1"), &serverAddr.sin_addr.s_addr);

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cout << "Bind failed with error code : " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	std::cout << "Bind success" << std::endl;

	if (listen(serverSocket, 10) == SOCKET_ERROR) {
		std::cout << "Listen failed with error code : " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	std::cout << "Server Listening on Port: " << portNumber << std::endl;

	vector<thread> threads; // vector to store threads
	while (1) {
		printf("Waiting for incoming connection\n");

		SOCKET acceptSocket;
		acceptSocket = accept(serverSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET) {
			std::cout << "Accept failed with error code : " << WSAGetLastError() << std::endl;
			closesocket(serverSocket);
			WSACleanup();
			return 0;
		}
		std::cout << "Accept Success" << std::endl;

		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		char clientIP[INET_ADDRSTRLEN];

		if (getpeername(acceptSocket, (sockaddr*)&clientAddr, &clientAddrSize) == SOCKET_ERROR) {
			std::cerr << "getpeername failed with error: " << WSAGetLastError() << std::endl;
		}
		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
			printf("Client IP Address: %s, Client Port: %d\n", clientIP, ntohs(clientAddr.sin_port));
		}
		try {
			threads.emplace_back(thread(SendFile, acceptSocket)); // create a new thread and store it in vector
			
			//SendFile(acceptSocket);
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	for (auto& t : threads)
		t.join();

	closesocket(serverSocket);
	WSACleanup();
	return 0;
}