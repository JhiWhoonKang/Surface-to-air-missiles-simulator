
// #include "SerialLS.h"
// #include "LCManager.h"
// #include "Serializer.h"
// #include "MessageParser.h"
// #include <iostream>
// #include <fcntl.h>
// #include <unistd.h>
// #include <termios.h>
// #include <thread>

// SerialLS::SerialLS(const std::string& port) : port_(port) {}

// void SerialLS::setCallback(IReceiverCallback *cb) {
//     callback_ = cb;
// }

// void SerialLS::start() {
//     // std::thread(&SerialLS::receiveLoop, this).detach();
// }

// SenderType SerialLS::getSenderType() const {
//     return SenderType::LS;
// }

// int SerialLS::openPort() const {
//     int fd = open(port_.c_str(), O_RDWR | O_NOCTTY);
//     if (fd < 0) return -1;

//     termios tty{};
//     tcgetattr(fd, &tty);
//     cfsetospeed(&tty, B115200);
//     cfsetispeed(&tty, B115200);
//     tty.c_cflag |= (CLOCAL | CREAD);
//     tty.c_cflag &= ~CSIZE;
//     tty.c_cflag |= CS8;
//     tty.c_cflag &= ~PARENB;
//     tty.c_cflag &= ~CSTOPB;
//     tty.c_cflag &= ~CRTSCTS;
//     tcsetattr(fd, TCSANOW, &tty);
//     return fd;
// }

// void SerialLS::receiveLoop() {
//     int fd = openPort();
//     if (fd < 0) {
//         std::cerr << "[SerialLS] 포트 열기 실패\n";
//         return;
//     }

//     std::cout << "[SerialLS] 수신 시작 (" << port_ << ")\n";
//     while (true) {
//         uint8_t buffer[512];
//         ssize_t len = read(fd, buffer, sizeof(buffer));
//         if (len <= 0) {
//             std::cerr << "[SerialLS] 수신 실패 또는 종료\n";
//             break;
//         }

//         std::vector<uint8_t> raw(buffer, buffer + len);
//         auto msg = Common::MessageParser::parse(raw, getSenderType());

//         if (std::holds_alternative<std::monostate>(msg.payload)) {
//             std::cerr << "[SerialLS] 메시지 파싱 실패\n";
//             continue;
//         }

//         if (callback_) callback_->onMessage(msg);
//     }

//     close(fd);
// }

// void SerialLS::sendRaw(const std::vector<uint8_t>& data, const std::string& logPrefix) {
//     int fd = openPort();
//     if (fd < 0) {
//         std::cerr << logPrefix << " - 포트 열기 실패\n";
//         return;
//     }

//     ssize_t written = write(fd, data.data(), data.size());
//     if (written < 0)
//         std::cerr << logPrefix << " - 전송 실패\n";
//     else
//         std::cout << logPrefix << " - 전송 완료 (" << written << " 바이트)\n";

//     close(fd);
// } 

// void SerialLS::sendStatus(const SystemStatus& status) {
//     auto data = Common::Serializer::serializeStatusResponse(status);
//     sendRaw(data, "[SerialLS] 상태 전송");
// }

// void SerialLS::sendStatus(const Common::CommonMessage& msg) {
//     auto raw = Common::Serializer::serializeMessage(msg);
//     sendRaw(raw, "[SerialLS] 일반 메시지 전송");
// }

// // IStatusSender 인터페이스용 기본 전송
// void SerialLS::sendRaw(const std::vector<uint8_t>& data) {
//     sendRaw(data, "[SerialLS] 기본 전송");
// }

// void SerialLS::handleReceived(const std::vector<uint8_t>& data, SenderType from) {
//     auto msg = Common::MessageParser::parse(data, from);

//     if (std::holds_alternative<std::monostate>(msg.payload)) {
//         std::cerr << "[SerialLS] handleReceived: 파싱 실패\n";
//         return;
//     }

//     if (callback_) callback_->onMessage(msg);
// }