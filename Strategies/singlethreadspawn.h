#ifndef SINGLETHREADSPAWN_H
#define SINGLETHREADSPAWN_H
#include <charconv>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include "../data.h"
#include "istrategy.h"

class SingleThreadSpawn final: public IStrategy
{
public:
    ~SingleThreadSpawn() override = default;

    void parse(const StratInfo& data, StratResult& result) override
    {
        result.name.append("Single Thread Spawn");

        std::thread only_thread([&, view = data.view]
        {
            std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
            std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

            std::unordered_map<std::string_view, Data> map;
            uint32_t value = 0;
            uint64_t total_lines = 0;
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
                        auto& [sum, max, min, count] = map.try_emplace(view.substr(city.first, city.second)).first->second;

                        value = parse_value_view(view, temp);

                        min = std::min(value, min);
                        max = std::max(value, max);
                        sum += value;
                        ++count;

                        city.first = i + 1;
                        ++total_lines;
                        break;
                    }
                default: break;
                }
            }
            result.total_lines = total_lines;
        });

        if(only_thread.joinable())
        {
            only_thread.join();
        }
    }
};

#endif //SINGLETHREADSPAWN_H
