#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define MAX_NETWORK_INPUT_BUFFER_SIZE 2048

class TcpListner {
public:
	TcpListner() = default;
	~TcpListner() = default;

	int Initialize(const char* ipAddress, int port);
	int Update();
	int Shutdown();

	void BroadcastToConnectedClients(int sendingClient, const char* message, int messageLength);
	void BroadcastToConnectedClients(const char* message, int messageLength);
	void SendMessageToClient(int client, const char* message, int messageLength);

protected:
	virtual void OnClientConnected(int client);
	virtual void OnClientDisconnected(int client);
	virtual void OnMessageReceived(int client, const char* message, int messageLength);

private:
	const char* m_ipAddress;
	int m_port;
	int m_socket;
	fd_set m_masterSet;
};
