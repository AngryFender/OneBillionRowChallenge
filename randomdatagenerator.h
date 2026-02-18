#ifndef RANDONDATAGENERATOR_H
#define RANDONDATAGENERATOR_H
#include <cstdint>
#include <fstream>

class RandomDataGenerator {
public:
    explicit RandomDataGenerator(const char* input_file_path, const char* output_file_path): _input_file(input_file_path), _output_file(output_file_path)
    {

    }

    bool generate(const uint32_t line_limit);

private:
    std::ifstream _input_file;
    std::ofstream _output_file;
};



#endif //RANDONDATAGENERATOR_H
