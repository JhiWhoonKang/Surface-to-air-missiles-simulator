#include "StepMotorController.hpp"

StepMotorController::StepMotorController()
{
#ifdef BUILD_FOR_PETALINUX
    if (loadMfrConfig("../config/MFR.ini", mfrConfig))
    {

        device = mfrConfig.device;
        uartBaudRate = mfrConfig.uartBaudRate;

        if (!initUart())
        {
        }
    }
#endif
<<<<<<< HEAD
std::cout <<"here"<<std::endl;
    // serverIp = mfrConfig.motorServerIp;
    // serverPort = mfrConfig.motorServerPort;
    serverIp = "127.0.0.1";
    serverPort = 8080;
=======

>>>>>>> 2e72494db2da0a5bbc3b0b7b21ffbd6c40999ffb
    connectToServer();
}

StepMotorController::~StepMotorController()
{
    if (uart_fd >= 0)
    {
        close(uart_fd);
    }

    if (sock_fd >= 0)
    {
        close(sock_fd);
    }
}

bool StepMotorController::initUart()
{
    uart_fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (uart_fd < 0)
    {
        std::cerr << "[UART] Failed to open device " << device << std::endl;
        return false;
    }

    struct termios tty;
    if (tcgetattr(uart_fd, &tty) != 0)
    {
        std::cerr << "[UART] Error getting termios attributes" << std::endl;
        return false;
    }

    cfsetospeed(&tty, uartBaudRate);
    cfsetispeed(&tty, uartBaudRate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                     // disable break processing
    tty.c_lflag = 0;                            // no signaling chars, no echo
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN] = 1;                         // read blocks
    tty.c_cc[VTIME] = 1;                        // timeout 0.1s

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);        // ignore modem controls
    tty.c_cflag &= ~(PARENB | PARODD);      // no parity
    tty.c_cflag &= ~CSTOPB;                 // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                // no hardware flow control

    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0)
    {
        std::cerr << "[UART] Error setting termios attributes" << std::endl;
        return false;
    }

    return true;
}

void StepMotorController::sendCommand(const std::string &cmd)
{
    if (uart_fd < 0)
    {
        // std::cerr << "[UART] UART not initialized" << std::endl;
        return;
    }
    std::string commandWithNewline = cmd + "\n";
    write(uart_fd, commandWithNewline.c_str(), commandWithNewline.size());
}

void StepMotorController::connectToServer()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        std::cerr << "[TCP] Socket creation failed\n";
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIp.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "[TCP] Invalid IP address: " << serverIp << "\n";
    }

    std::cout << "[TCP] Trying to connect to " << serverIp << ":" << serverPort << "...\n";

    int connect_count = 5;
    for (int i = 0; i < connect_count; ++i)
    {
        if (sock_fd < 0)
        {
            std::cerr << "[TCP] Socket not created\n";
            return;
        }

        if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0)
        {
            std::cout << "[TCP] Connected to server\n";
            return;
        }

        std::cerr << "[TCP] Connection failed. Retrying in 1 second...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void StepMotorController::sendCmd(const std::string &cmd)
{
    if (sock_fd < 0)
    {
        // std::cerr << "[TCP] Socket not connected\n";
        return;
    }

    std::string data = cmd + "\n";
    ssize_t sent = send(sock_fd, data.c_str(), data.length(), 0);
    if (sent < 0)
    {
        std::cerr << "[TCP] Failed to send command\n";
    }
}