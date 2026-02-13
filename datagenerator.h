#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>


class DataGenerator {
public:
    explicit DataGenerator(const char* path): _file(path,std::ios::in | std::ios::out)
    {
    }

    bool generate(const uint32_t line_limit);

private:
    std::fstream _file;
};



#endif //DATAGENERATOR_H
