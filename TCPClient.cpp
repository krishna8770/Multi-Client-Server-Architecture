#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
using namespace std;
#define BUFFER_SIZE 1024

#pragma comment(lib, "Ws2_32.lib")

int main() {

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);

	int portNumber = 8888;

	printf("TCP Client\n");

	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		std::cout << "WSAStartup failed" << std::endl;
		return 0;
	}
	cout << "WSAStartup success" << endl;
	cout << "Status : " << wsaData.szSystemStatus << endl;
	cout << "Status : " << wsaData.szDescription << endl;

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cout << "Create socket failed with error code : " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	std::cout << "Create socket success" << std::endl;

	std::cout << "TCP Client trying to Connect to server" << std::endl;

	sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8888);
	InetPtonA(AF_INET, PCSTR("127.0.0.1"), &serverAddr.sin_addr.s_addr);

	if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cout << "Connect to server failed with error code : " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}
	std::cout << "TCP Client Connected to server successfully" << std::endl;

	sockaddr_in clientAddr;
	int addrSize = sizeof(clientAddr);
	if (getsockname(clientSocket, (sockaddr*)&clientAddr, &addrSize) == SOCKET_ERROR) {
		std::cerr << "getsockname failed with error code: " << WSAGetLastError() << std::endl;
	}
	else {
		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);  // Convert binary IP to readable format
		int clientPort = ntohs(clientAddr.sin_port);  // Convert port to host byte order
		printf("TCP Client has IP Address: %s and Port Number: %d\n", clientIP, clientPort);
	}

	string fileName = "CancelledChequeRecieved.png";
	string filePath = "C:/Users/knarwani/PracticeProjects/TCPClient/Recieve File/" + fileName;
	ofstream file(filePath, std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << fileName << std::endl;
		closesocket(clientSocket);
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}
	printf("File opened successfully to Write\n");

	char readBuffer[1024];
	int bytesRead;
	while (bytesRead = recv(clientSocket, readBuffer, BUFFER_SIZE, 0)) {
		file.write(readBuffer, bytesRead);
	}
	file.close();

	if (bytesRead < 0)
	{
		std::cout << "Receive failed with error code : " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}
	cout << "File received successfully" << endl;
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}