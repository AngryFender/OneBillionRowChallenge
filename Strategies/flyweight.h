#ifndef FLYWEIGHT_H
#define FLYWEIGHT_H
#include <string>
#include <unordered_map>

#include "istrategy.h"
#include "../data.h"
#include "../helper.h"
#include <boost/flyweight.hpp>

class Flyweight: public IStrategy
{
public:
    Flyweight() = default;
    ~Flyweight() override = default ;

    void parse(const StratInfo& data, StratResult& result) override
    {
        result.name.append("Boost Flyweight");
        std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
        std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

        const std::string_view view = data.view;
        using Location = boost::flyweight<std::string>;
        std::unordered_map<Location, Data> map;

        uint32_t value = 0;
        uint64_t counts = 0;

        for (size_t i = 0; i < data.file_size; ++i)
        {
            switch (view[i])
            {
            case ';':
                {
                    city.second = i - city.first;
                    temp.first = i + 1;
                    break;
                }
            case '\n':
                {
                    temp.second = i - temp.first;
                    std::string_view location_view = view.substr(city.first, city.second);
                    Location fly(location_view);
                    auto& [sum, max, min, count] = map.try_emplace(fly).first->second;

                    // parse float using from_chars -> value
                    value = parse_value_view(view, temp);

                    min = std::min(value, min);
                    max = std::max(value, max);
                    sum += value;
                    ++count;

                    city.first = i + 1;
                    ++counts;
                    break;
                }
            default: break;
            }
        }
        result.total_lines = counts;
    }
};


#endif //FLYWEIGHT_H
