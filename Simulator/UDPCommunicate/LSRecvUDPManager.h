#ifndef LS_RECV_UDP_MANAGER_H
#define LS_RECV_UDP_MANAGER_H

#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>

class LSRecvUDPManager
{
public:
	LSRecvUDPManager();
	~LSRecvUDPManager();

	// Initialize the UDP socket
	bool LSSocketOpen(int port);

	// Receive data from the socket
	int receiveData(std::vector<uint8_t> *buffer, int bufferSize);

	// Close the socket
	void closeSocket();

private:
	int ls_socket_;					 // Socket file descriptor
	struct sockaddr_in server_addr_; // Server address structure
	socklen_t addr_len_;			 // Length of the address structure
	char buffer_[1024];				 // Buffer for receiving data
	int buffer_size_;				 // Size of the buffer
	int recv_len_;					 // Length of the received data
	std::thread recv_thread_;		 // Thread for receiving data
};

#endif // LS_RECV_UDP_MANAGER_H