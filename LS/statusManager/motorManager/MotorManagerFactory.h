#pragma once
#include "MotorManagerInterface.h"
#include "LSMotorManager.h"
#include "TestMotorManager.h"
#include <memory>
#include <string>

class MotorManagerFactory {
public:
    static std::unique_ptr<MotorManagerInterface> create(const std::string& type,
                                                        const std::string& IP, const int& port) {
        if (type == "REAL") {
            return std::make_unique<LSMotorManager>(IP, port);
        } else if (type == "TEST") {
            return std::make_unique<TestMotorManager>();
        } else {
            throw std::invalid_argument("Unknown MotorManager type: " + type);
        }
    }
};