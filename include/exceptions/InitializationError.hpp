#pragma once
#include <stdexcept>
#include <string>
class InitializationError : public std::runtime_error {
   public:
   InitializationError(const std::string& what_arg): std::runtime_error(what_arg){};
};
