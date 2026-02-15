#include "randomdatagenerator.h"
#include <iostream>
#include <sstream>
#include <unordered_map>

bool RandomDataGenerator::generate(const uint32_t line_limit)
{
    if(!_file || !_file.is_open())
    {
        return false;
    }

    uint32_t current_line_limit = 0;
    std::string line_content;
    std::unordered_map<std::string, int> mapPlace;
    std::unordered_map<int, std::string> mapData;
    mapPlace.reserve(10000);
    mapData.reserve(10000);

    while(std::getline(_file, line_content,'\n'))
    {
        //TODO create entry with id and place
        ++current_line_limit;
    }

    return true;
}
