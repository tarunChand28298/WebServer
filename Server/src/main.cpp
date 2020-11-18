#include "TCP_Listner/TCPListner.h"

#include <Windows.h>
#include <iostream>

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

#define CONTENT_BASE_DIR (std::string("C:\\Users\\TarunChand\\Desktop\\WebContent"))

void changeColor(int desiredColor) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), desiredColor);}
void ltrim(std::string& s) { s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); })); }
void rtrim(std::string& s) { s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());}
void trim(std::string& s) { ltrim(s); rtrim(s); }

enum class ResourceType
{
	Home,
	Unsupported,
	Text,
	Html,
	Css,
	Javascript,
	Image,
	Icon,
	Audio,
	Video
};

ResourceType GetResourceType(std::string requestedPath)
{
	if (requestedPath == "/")
	{
		return ResourceType::Home;
	}
	else
	{
		std::string extension = requestedPath.substr(requestedPath.find('.') + 1, requestedPath.size() - 1);
		if (extension == "txt") return ResourceType::Text;
		else if (extension == "html") return ResourceType::Html;
		else if (extension == "css") return ResourceType::Css;
		else if (extension == "js") return ResourceType::Javascript;
		else if (extension == "jpg") return ResourceType::Image;
		else if (extension == "ico") return ResourceType::Icon;
		else if (extension == "mp3") return ResourceType::Audio;
		else if (extension == "mp4") return ResourceType::Video;
		else return ResourceType::Unsupported;
	}
}
std::string GetContentType(ResourceType rType)
{
	switch (rType)
	{
		case ResourceType::Home: return "text/html";
		case ResourceType::Unsupported: return "text/plain";
		case ResourceType::Text: return "text/plain";
		case ResourceType::Html: return "text/html";
		case ResourceType::Css: return "text/css";
		case ResourceType::Javascript: return "text/javascript";
		case ResourceType::Image: return "image/jpeg";
		case ResourceType::Icon: return "image/webp";
		case ResourceType::Audio: return "audio/mpeg";
		case ResourceType::Video: return "video/mp4";
		default: return "text/plain";
	}
}
std::string GetContentFilename(ResourceType type, std::string file)
{
	switch (type)
	{
		case ResourceType::Home: return CONTENT_BASE_DIR + "/index.html";
		case ResourceType::Unsupported: return CONTENT_BASE_DIR + "/UnsupportedMessage.txt";
		case ResourceType::Text:
		case ResourceType::Html:
		case ResourceType::Css:
		case ResourceType::Javascript:
		case ResourceType::Image:
		case ResourceType::Icon:
		case ResourceType::Audio:
		case ResourceType::Video: return CONTENT_BASE_DIR + file;
		default: return CONTENT_BASE_DIR + "/UnsupportedMessage.txt";
	}
}

const std::map<std::string, std::string> statusCodes =
{
	{"100", "Continue"},
	{"101", "Switching Protocols"},
	{"200", "OK"},
	{"201", "Created"},
	{"202", "Accepted"},
	{"203", "Non - Authoritative Information"},
	{"204", "No Content"},
	{"205", "Reset Content"},
	{"206", "Partial Content"},
	{"300", "Multiple Choices"},
	{"301", "Moved Permanently"},
	{"302", "Found"},
	{"303", "See Other"},
	{"304", "Not Modified"},
	{"305", "Use Proxy"},
	{"307", "Temporary Redirect"},
	{"400", "Bad Request"},
	{"401", "Unauthorized"},
	{"402", "Payment Required"},
	{"403", "Forbidden"},
	{"404", "Not Found"},
	{"405", "Method Not Allowed"},
	{"406", "Not Acceptable"},
	{"407", "Proxy Authentication Required"},
	{"408", "Request Time - out"},
	{"409", "Conflict"},
	{"410", "Gone"},
	{"411", "Length Required"},
	{"412", "Precondition Failed"},
	{"413", "Request Entity Too Large"},
	{"414", "Request - URI Too Large"},
	{"415", "Unsupported Media Type"},
	{"416", "Requested range not satisfiable"},
	{"417", "Expectation Failed"},
	{"500", "Internal Server Error"},
	{"501", "Not Implemented"},
	{"502", "Bad Gateway"},
	{"503", "Service Unavailable"},
	{"504", "Gateway Time - out"},
	{"505", "HTTP Version not supported"}
};
struct HTTPRequestHeader
{
	std::string method;
	std::string path;
	std::string version;

	std::map<std::string, std::string> headerFields;
};
struct HTTPResponseHeader
{
	std::string version;
	std::pair<std::string, std::string> statusCode;
	std::map<std::string, std::string> headerFields;
};

class WebServer : public TcpListner
{
	void ParseRequest(const char* inRequest, HTTPRequestHeader& outRequest)
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

			std::string key, value;
			std::getline(field, key, ':');
			std::getline(field, value);
			trim(key); trim(value);

			outRequest.headerFields.insert({ key, value });
		}
	}
	void DeliverFile(const int client, const std::string fileName, HTTPResponseHeader& header)
	{
		std::stringstream fullResponse;

		std::fstream fileToSend(fileName, std::ios::binary | std::ios::in);
		std::stringstream responseBodyStream;
		responseBodyStream << fileToSend.rdbuf();
		header.headerFields["Content-Length"] = std::to_string(responseBodyStream.str().size());

		std::stringstream responseHeaderStream;
		responseHeaderStream << header.version << " " << header.statusCode.first << " " << header.statusCode.second << "\r\n";
		for (auto pair : header.headerFields)
		{
			responseHeaderStream << pair.first << ": " << pair.second << "\r\n";
		}

		fullResponse << responseHeaderStream.str();
		fullResponse << "\r\n";
		fullResponse << responseBodyStream.str();
		SendMessageToClient(client, fullResponse.str().c_str(), fullResponse.str().size() + 1);
	}

	void OnClientConnected(int client) override
	{

	}

	void OnClientDisconnected(int client) override
	{
		
	}

	void OnMessageReceived(int client, const char* message, int messageLength) override
	{
		HTTPRequestHeader httpRequest;
		ParseRequest(message, httpRequest);

		changeColor(FOREGROUND_BLUE);
		std::cout << "Client [" << client << "]:" << std::endl;
		changeColor(FOREGROUND_GREEN);
		std::cout << httpRequest.method << " " << httpRequest.path << " " << httpRequest.version << std::endl;
		changeColor(7);
		for (auto pair : httpRequest.headerFields)
		{
			std::cout << pair.first << " : " << pair.second << std::endl;
		}
		std::cout << std::endl;

		HTTPResponseHeader header;
		header.version = "HTTP/1.1";
		header.statusCode = *statusCodes.find("200");
		header.headerFields["Cache-Control"] = "no-cache, private";
		header.headerFields["Accept-Ranges"] = "bytes";
		header.headerFields["Content-Type"] = GetContentType(GetResourceType(httpRequest.path));

		DeliverFile(client, GetContentFilename(GetResourceType(httpRequest.path), httpRequest.path), header);
	}
};

#include "Thread/thread.h"

void greet(const char* msg) {
	std::cout << msg << std::endl;
}


int main()
{

	waves::thread t1(greet, "hello");
	{
		waves::thread t2(greet, "bye");
	}

	t1.join();

	WebServer server;
	server.Initialize("0.0.0.0", 54000);

	while (1)
	{
		server.Update();
	}

	server.Shutdown();

	return 0;
}
