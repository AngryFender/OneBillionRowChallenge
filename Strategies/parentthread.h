#ifndef PARENTTHREAD_H
#define PARENTTHREAD_H
#include <charconv>
#include <string>
#include <string_view>
#include <unordered_map>
#include "../data.h"
#include "istrategy.h"

class ParentThread final: public IStrategy
{
public:
    ~ParentThread() override = default;

    void parse(const StratInfo& data, StratResult& result) override
    {
        result.name.append("Parent Thread");
        std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
        std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

        std::unordered_map<std::string_view, Data> map;
        const std::string_view view = data.view;
        std::string_view value_view;
        double value = 0;

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
                    auto& [max, min,mean, count] = map.try_emplace(view.substr(city.first, city.second)).first->second;

                    value_view = view.substr(temp.first, i - temp.first);
                    auto [ptr, ec] = std::from_chars(value_view.data(), value_view.data() + value_view.size(), value);

                    if (ec != std::errc())
                    {
                        continue;
                    }
                    min = std::min(value, min);
                    max = std::max(value, max);
                    mean += (value - mean) / static_cast<double>(++count);

                    city.first = i + 1;
                    ++result.total_lines;
                    break;
                }
            default: break;
            }
        }
    }
};

#endif //PARENTTHREAD_H
