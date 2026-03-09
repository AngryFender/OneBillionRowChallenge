#include "randomdatagenerator.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <bits/algorithmfwd.h>
#include <algorithm>
#include <format>
#include <iomanip>
#include <sstream>

bool RandomDataGenerator::generate(const uint32_t line_limit)
{
    if (!_input_file || !_input_file.is_open() || !_output_file.is_open())
    {
        return false;
    }

    uint32_t current_line_counts = 0;
    std::unordered_set<std::string> setPlace;
    std::vector<std::string> vecData;
    setPlace.reserve(1000);
    vecData.reserve(1000);

    std::string place;
    std::string value;
    int place_id = 0;

    while (std::getline(_input_file, place, ';') && std::getline(_input_file, value, '\n'))
    {
        if (!setPlace.contains(place))
        {
            setPlace.insert(place);
            vecData.push_back(place);
            ++place_id;
        }
        ++current_line_counts;
    }
    std::cout << "Total number of places = " << setPlace.size();

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> id_dist(0, place_id);
    std::normal_distribution<double> temp_dist(10.0, 10.0);

    _output_file.clear();
    _output_file.seekp(0, std::ios::beg);

    int random_place_id = 0;
    double raw_temp = 0.0;
    double random_temp = 0.0;

    std::string chunk;
    chunk.reserve(LINE_CHUNK);
    auto output_it = std::back_inserter(chunk);

    int chunk_limit = 0;
    for (int count = 0; count < line_limit; ++count)
    {
        random_place_id = id_dist(gen);
        raw_temp = temp_dist(gen);
        random_temp = std::clamp<double>(raw_temp, LOWEST_TEMP, HIGHEST_TEMP);
        std::format_to(output_it, "{};{:.4f}\n",vecData[random_place_id], random_temp);
        if (++chunk_limit >= LINE_CHUNK)
        {
            _output_file << chunk;
            chunk_limit = 0;
            chunk.clear();
            output_it = std::back_inserter(chunk);
        }
    }
    return true;
}
