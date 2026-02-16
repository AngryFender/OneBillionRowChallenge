#include "randomdatagenerator.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <bits/algorithmfwd.h>
#include <algorithm>

bool RandomDataGenerator::generate(const uint32_t line_limit)
{
    if (!_file || !_file.is_open())
    {
        return false;
    }

    uint32_t line_counts = 0;
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
        ++line_counts;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> id_dist(1, place_id);
    std::normal_distribution<double> temp_dist(10.0, 10.0);

    int random_id = id_dist(gen);
    auto raw_temp = temp_dist(gen);
    auto random_temp = std::clamp<double>(raw_temp, -20.0, 40.0);

    std::cout<<"random place id = "<< random_id << "random temp = "<<random_temp;

    return true;
}
