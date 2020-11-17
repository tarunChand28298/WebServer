#include "TCPListner.h"

int TcpListner::Initialize(const char* ipAddress, int port)
{
	int returnValue;

	//Initialize private fields:
	m_ipAddress = ipAddress;
	m_port = port;

	//Initialize winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	returnValue = WSAStartup(version, &data);
	if (returnValue != 0) { return returnValue; }

	//Create socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET) { return WSAGetLastError(); }

	//Bind the socket to address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	inet_pton(AF_INET, m_ipAddress, &hint.sin_addr);
	hint.sin_port = htons(m_port);

	returnValue = bind(m_socket, (sockaddr*)&hint, sizeof(hint));
	if (returnValue == SOCKET_ERROR) { return WSAGetLastError(); }

	FD_ZERO(&m_masterSet);
	FD_SET(m_socket, &m_masterSet);

	//start listening for connections
	listen(m_socket, SOMAXCONN);

	return 0;
}

int TcpListner::Update()
{
	fd_set localSet = m_masterSet;
	int socketCount = select(0, &localSet, nullptr, nullptr, nullptr);

	for (int i = 0; i < socketCount; i++) {
		SOCKET currentSocket = localSet.fd_array[i];
		if (currentSocket == m_socket)
		{
			//Accept a new connection.
			SOCKET client = accept(m_socket, nullptr, nullptr);
			FD_SET(client, &m_masterSet);

			OnClientConnected(client);
		}
		else
		{
			//Accept a new message.
			char buffer[MAX_NETWORK_INPUT_BUFFER_SIZE];
			ZeroMemory(buffer, MAX_NETWORK_INPUT_BUFFER_SIZE);

			int bytesReceived = recv(currentSocket, buffer, MAX_NETWORK_INPUT_BUFFER_SIZE, 0);

			if (bytesReceived <= 0)
			{
				//Client has disconnected
				OnClientDisconnected(currentSocket);

				closesocket(currentSocket);
				FD_CLR(currentSocket, &m_masterSet);
			}
			else
			{
				//Message received
				OnMessageReceived(currentSocket, buffer, bytesReceived);
			}
		}
	}

	return 0;
}

int TcpListner::Shutdown()
{
	while (m_masterSet.fd_count > 0)
	{
		SOCKET currentSocket = m_masterSet.fd_array[0];
		FD_CLR(currentSocket, &m_masterSet);
		closesocket(currentSocket);
	}

	WSACleanup();

	return 0;
}

void TcpListner::OnClientConnected(int client)
{
}

void TcpListner::OnClientDisconnected(int client)
{
}

void TcpListner::OnMessageReceived(int client, const char* message, int messageLength)
{
}

void TcpListner::SendMessageToClient(int client, const char* message, int messageLength)
{
	send(client, message, messageLength, 0);
}

void TcpListner::BroadcastToConnectedClients(int sendingClient, const char* message, int messageLength)
{
	for (int i = 0; i < m_masterSet.fd_count; i++) {
		SOCKET outSocket = m_masterSet.fd_array[i];
		if (outSocket != m_socket && outSocket != sendingClient) {
			send(outSocket, message, messageLength, 0);
		}
	}
}

void TcpListner::BroadcastToConnectedClients(const char* message, int messageLength)
{
	for (int i = 0; i < m_masterSet.fd_count; i++) {
		SOCKET outSocket = m_masterSet.fd_array[i];
		if (outSocket != m_socket) {
			send(outSocket, message, messageLength, 0);
		}
	}
}
