#include "TCP_Listner/TCPListner.h"

#include <Windows.h>
#include <iostream>

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

void changeColor(int desiredColor) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), desiredColor);}
void ltrim(std::string& s) { s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); })); }
void rtrim(std::string& s) { s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());}
void trim(std::string& s) { ltrim(s); rtrim(s); }

struct KeyValuePair
{
	std::string key;
	std::string value;
};

struct HTTPRequest
{
	std::string method;
	std::string path;
	std::string version;

	std::vector<KeyValuePair> headerFields;
};

void ParseRequest(const char* inRequest, HTTPRequest& outRequest)
{
	std::string messageString(inRequest); 
	trim(messageString);

	std::stringstream request(messageString);

	std::string method;
	std::getline(request, method);

	std::stringstream firstLine(method);
	std::getline(firstLine, outRequest.method, ' '); trim(outRequest.method);
	std::getline(firstLine, outRequest.path, ' '); trim(outRequest.path);
	std::getline(firstLine, outRequest.version, ' '); trim(outRequest.version);

	for (std::string line; std::getline(request, line);)
	{
		std::stringstream field(line);
		KeyValuePair newPair;

		std::getline(field, newPair.key, ':');
		std::getline(field, newPair.value);
		trim(newPair.key); trim(newPair.value);

		outRequest.headerFields.push_back(newPair);
	}
}

class WebServer : public TcpListner
{
	void OnClientConnected(int client) override
	{

	}

	void OnClientDisconnected(int client) override
	{
		
	}

	void OnMessageReceived(int client, const char* message, int messageLength) override
	{
		HTTPRequest httpRequestStruct;
		ParseRequest(message, httpRequestStruct);

		//std::fstream imageFile("C:\\Users\\TarunChand\\Desktop\\bunny.mp4", std::ios::binary | std::ios::in);
		//std::stringstream responseContentStream;
		//responseContentStream << imageFile.rdbuf();
		//std::string response = responseContentStream.str();
		//std::stringstream responseStream;
		//responseStream << "HTTP/1.1 200 OK\r\n";
		//responseStream << "Cache-Control: no-cache, private\r\n";
		//responseStream << "Accept-Ranges: bytes\r\n";
		//responseStream << "Content-Type: video/mp4\r\n";
		//responseStream << "Content-Length : " << response.size() << "\r\n";
		//responseStream << "\r\n";
		//responseStream << response;

		std::stringstream responseStream;
		std::string response = "hello";
		responseStream << "HTTP/1.1 200 OK\r\n";
		responseStream << "Cache-Control: no-cache, private\r\n";
		responseStream << "Accept-Ranges: bytes\r\n";
		responseStream << "Content-Type: text/plain\r\n";
		responseStream << "Content-Length : " << response.size() << "\r\n";
		responseStream << "\r\n";
		responseStream << response;
		SendMessageToClient(client, responseStream.str().c_str(), responseStream.str().size() + 1);
	}
};

int main()
{
	WebServer server;
	server.Initialize("0.0.0.0", 54000);

	while (1)
	{
		server.Update();
	}

	server.Shutdown();

	return 0;
}
