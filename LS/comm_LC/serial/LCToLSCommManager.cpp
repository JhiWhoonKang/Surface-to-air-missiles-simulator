#include "LCToLSCommManager.h"
#include "ConfigParser.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <thread>

LCToLSCommManager::LCToLSCommManager(SerialReceiverInterface& receiver, const std::string& configPath)
    : receiver(receiver)
{
    init(configPath); // UART 초기화
    listenerThread = std::thread(&LCToLSCommManager::run, this); // 수신 루프 스레드 실행
}

LCToLSCommManager::~LCToLSCommManager()
{
    if (listenerThread.joinable())
    {
        listenerThread.join();
    }
}

int LCToLSCommManager::configureUART(const std::string& devicePath, int baudRate, int dataBits, int stopBits, char parity)
{
    int fd = open(devicePath.c_str(), O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror("[configureUART] Failed to open UART device");
        return -1;
    }
    if (!isatty(fd))
    {
        std::cerr << "[configureUART] Not a TTY device: " << devicePath << "\n";
    }
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        perror("[configureUART] tcgetattr failed");
        close(fd);
        return -1;
    }

    speed_t speed;
    switch (baudRate)
    {
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        default:
            std::cerr << "[configureUART] Unsupported baud rate: " << baudRate << "\n";
            close(fd);
            return -1;
    }
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);

    tty.c_cflag &= ~CSIZE;
    switch (dataBits) 
    {
        case 5: tty.c_cflag |= CS5; break;
        case 6: tty.c_cflag |= CS6; break;
        case 7: tty.c_cflag |= CS7; break;
        case 8: tty.c_cflag |= CS8; break;
        default:
            std::cerr << "[configureUART] Unsupported data bits: " << dataBits << "\n";
            close(fd);
            return -1;
    }

    switch (parity) 
    {
        case 'N': case 'n': tty.c_cflag &= ~PARENB; break;
        case 'E': case 'e': tty.c_cflag |= PARENB; tty.c_cflag &= ~PARODD; break;
        case 'O': case 'o': tty.c_cflag |= PARENB; tty.c_cflag |= PARODD; break;
        default:
            std::cerr << "[configureUART] Unsupported parity: " << parity << "\n";
            close(fd);
            return -1;
    }

    if (stopBits == 1) tty.c_cflag &= ~CSTOPB;
    else if (stopBits == 2) tty.c_cflag |= CSTOPB;
    else 
    {
        std::cerr << "[configureUART] Unsupported stop bits: " << stopBits << "\n";
        close(fd);
        return -1;
    }

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) 
    {
        perror("[configureUART] tcsetattr failed");
        close(fd);
        return -1;
    }

    return fd;
}

void LCToLSCommManager::init(const std::string& configPath)
{
    try 
    {
        std::string device = ConfigParser::getValue("FireControlCommSerial", "DevicePath", configPath);
        int baud = ConfigParser::getInt("FireControlCommSerial", "BaudRate", configPath);
        int dataBits = ConfigParser::getInt("FireControlCommSerial", "DataBits", configPath);
        int stopBits = ConfigParser::getInt("FireControlCommSerial", "StopBits", configPath);
        std::string parityStr = ConfigParser::getValue("FireControlCommSerial", "Parity", configPath);

        if (parityStr.empty()) throw std::invalid_argument("Parity value missing");
        char parity = parityStr[0];

        fd = configureUART(device, baud, dataBits, stopBits, parity);
        serialPath = device;

        std::cout << "[LCToLSCommManager] Initialized with " << device
                  << ", baud " << baud << ", dataBits " << dataBits
                  << ", stopBits " << stopBits << ", parity " << parity << "\n";

    } 
    catch (const std::exception& e) 
    {
        std::cerr << "[LCToLSCommManager] Config error: " << e.what() << "\n";
        fd = -1;
    }
}

void LCToLSCommManager::run() 
{
    if (fd < 0) 
    {
        std::cerr << "[LCToLSCommManager] Cannot run: invalid file descriptor\n";
        return;
    }

    std::cout << "[LCToLSCommManager] Listening on " << serialPath << "\n";
    while (true) 
    {
        std::vector<uint8_t> buffer(sizeof(LauncherMessage));
        ssize_t len = read(fd, buffer.data(), buffer.size());

        if (len > 0) 
        {
            receiver.callBack(buffer);
        } 
        else if (len < 0) 
        {
            perror("[LCToLSCommManager] read error");
        }
    }
}

void LCToLSCommManager::sendData(const std::vector<uint8_t>& packet)
{
    if (fd < 0) 
    {
        std::cerr << "[LCToLSCommManager] Cannot send: invalid file descriptor\n";
        return;
    }

    ssize_t written = write(fd, packet.data(), packet.size());
    if (written < 0) 
    {
        perror("[LCToLSCommManager] write error");
    } 
    else if (static_cast<size_t>(written) < packet.size()) 
    {
        std::cerr << "[LCToLSCommManager] Partial write: " << written << " / " << packet.size() << "\n";
    } 
    else 
    {
        std::cout << "[LCToLSCommManager] Sent " << written << " bytes to serial\n";
    }
}
