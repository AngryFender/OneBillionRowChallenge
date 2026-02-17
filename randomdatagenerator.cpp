#include "randomdatagenerator.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <bits/algorithmfwd.h>
#include <algorithm>
#include <sstream>

bool RandomDataGenerator::generate(const uint32_t line_limit)
{
    if (!_file || !_file.is_open())
    {
        return false;
    }

    uint32_t current_line_counts = 0;
    std::unordered_set<std::string> setPlace;
    std::unordered_map<int, std::string> mapData;
    setPlace.reserve(10000);
    mapData.reserve(10000);

    std::string place;
    std::string value;
    int place_id = 0;

    while (std::getline(_file, place, ';') && std::getline(_file, value, '\n'))
    {
        if (!setPlace.contains(place))
        {
            setPlace.insert(place);
            mapData[++place_id] = place;
        }
        ++current_line_counts;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> id_dist(1, place_id);
    std::normal_distribution<double> temp_dist(10.0, 10.0);

    _file.clear();
    _file.seekp(0, std::ios::beg);

    std::stringstream buffer;
    buffer<<_file.rdbuf();
    const std::string file_content = buffer.str();

    const float target_multiple = static_cast<float>(line_limit) / static_cast<float>(current_line_counts);
    int random_place_id = 0;
    double raw_temp = 0.0;
    double random_temp = 0.0;

    for(float count = 1.0f ; count < target_multiple; ++count)
    {
         random_place_id = id_dist(gen);
         raw_temp = temp_dist(gen);
         random_temp = std::clamp<double>(raw_temp, -20.0, 40.0);
        _file << mapData[random_place_id] <<";"<<random_temp<<"\n";
    }

    std::cout<<target_multiple<<","<<current_line_counts<<std::endl;
    return true;


    return true;
}
