#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <functional>

class Server
{
public:
	Server(const std::string &address = "0.0.0.0", int port = 9000);
	~Server();

	void start();
	void stop();
	void restart();
	bool isRunning() const { return running; }

private:
	void serverLoop();
	void handleClient(int clientSocket);

	std::string serverAddress;
	int port;
	int serverSocket;
	std::atomic<bool> running;
	std::thread serverThread;
	std::vector<std::thread> clientThreads;
};