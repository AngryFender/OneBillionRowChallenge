#include "datagenerator.h"

bool DataGenerator::generate(const uint32_t line_limit)
{
    if(!_file || !_file.is_open())
    {
        return false;
    }

    uint32_t current_line_limit = 0;
    std::string line_content;
    while(std::getline(_file, line_content,'\n'))
    {
        ++current_line_limit;
    }

    _file.clear();
    _file.seekp(0, std::ios::beg);

    std::stringstream buffer;
    buffer<<_file.rdbuf();
    const std::string file_content = buffer.str();

    const float target_multiple = static_cast<float>(line_limit) / static_cast<float>(current_line_limit);
    for(float count = 1.0f ; count < target_multiple; ++count)
    {
        _file << file_content;
    }

    std::cout<<target_multiple<<","<<current_line_limit<<std::endl;
    return true;
}
