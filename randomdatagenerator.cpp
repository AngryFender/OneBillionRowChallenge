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
    if (!_input_file || !_input_file.is_open() || !_output_file.is_open())
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

    while (std::getline(_input_file, place, ';') && std::getline(_input_file, value, '\n'))
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

    _output_file.clear();
    _output_file.seekp(0, std::ios::beg);

    int random_place_id = 0;
    double raw_temp = 0.0;
    double random_temp = 0.0;

    std::stringstream chunk;
    int chunk_limit = 0;
    for (int count = 0; count < line_limit; ++count)
    {
        random_place_id = id_dist(gen);
        raw_temp = temp_dist(gen);
        random_temp = std::clamp<double>(raw_temp, -20.0, 40.0);
        chunk << mapData[random_place_id] << ";" << random_temp << "\n";
        if (chunk_limit++ < 10000000)
        {
            _output_file << chunk.rdbuf();
            chunk.str("");
            chunk_limit = 0;
        }
    }
    return true;
}
