#pragma once

#include <string>

class AppConfigProvider {
   public:
    virtual std::string biosFileLocation() = 0;
};

class EnvironmentVariableAppConfigProvider : public AppConfigProvider {
   public:
    std::string biosFileLocation() override;
};
