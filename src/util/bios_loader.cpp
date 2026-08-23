

#include <fstream>
#include <ios>
#include <string>
#include <util/bios_loader.hpp>
#include <exceptions/initialization_error.hpp>


std::vector<unsigned char> readProgram(const char* programName, const unsigned int maxCodeLen){
    std::ifstream file(programName,std::ios::binary);
    unsigned char ve[maxCodeLen];
    file.seekg(0,std::ios_base::end);
    auto len = file.tellg();
    file.seekg(0,std::ios_base::beg);
    if(len>maxCodeLen){
        throw InitializationError(std::string("Too many bytes ")+std::to_string(len));
    }

    file.read((char*)ve, maxCodeLen);
    file.close();

    return std::vector(ve,ve+maxCodeLen);
}
