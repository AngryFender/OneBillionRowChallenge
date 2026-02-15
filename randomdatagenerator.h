#ifndef RANDONDATAGENERATOR_H
#define RANDONDATAGENERATOR_H
#include <cstdint>
#include <fstream>

class RandomDataGenerator {
public:
    explicit RandomDataGenerator(const char* path): _file(path)
    {

    }

    bool generate(const uint32_t line_limit);

private:
    std::fstream _file;
};



#endif //RANDONDATAGENERATOR_H
