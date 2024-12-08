#include<string>
#include<exception>

class InitializationError : public std::exception {
    private:
    std::string message;
    public:
    InitializationError(std::string);
};
