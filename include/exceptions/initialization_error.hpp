#pragma once
#include <stdexcept>
#include <string>

class InitializationError : public std::runtime_error {
   public:
   InitializationError(const std::string& what_arg): std::runtime_error(what_arg){};
   InitializationError(const char* arg): std::runtime_error(arg) {};
};

class InvalidConfigurationError : public std::runtime_error {
    public:
    InvalidConfigurationError(const std::string& what_arg): std::runtime_error(what_arg){};
    InvalidConfigurationError(const char* arg): std::runtime_error(arg) {};
};
