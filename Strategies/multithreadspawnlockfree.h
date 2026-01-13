#ifndef MULTITHREADSPAWNLOCKFREE_H
#define MULTITHREADSPAWNLOCKFREE_H

#include <numeric>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../data.h"
#include "istrategy.h"

class MultiThreadSpawnLockFree final: public IStrategy
{
public:
    explicit MultiThreadSpawnLockFree(const int num_of_threads = 0):_thread_no(num_of_threads){}
    ~MultiThreadSpawnLockFree()  override = default;

    void parse(const StratInfo& data, StratResult& result) override
    {
        result.name.append(std::to_string(_thread_no) + " Threads Spawn");

        using map = std::unordered_map<std::string_view, Data>;
        std::vector<std::thread> thread_collection;
        thread_collection.reserve(_thread_no);
        std::vector<map> maps;

        float high = 0;
        float low = -1;
        const float factor = static_cast<float>(data.file_size) / static_cast<float>(_thread_no);

        std::vector<std::pair<float, float>> ranges;
        ranges.reserve(_thread_no);
        for (int t = 0; t < _thread_no; ++t)
        {
            ranges.emplace_back(++low * factor, std::min(++high * factor, static_cast<float>(data.file_size)));
            maps.push_back(std::unordered_map<std::string_view, Data>());
        }

        std::vector<uint64_t> line_count(_thread_no,0);
        for (int t = 0; t < _thread_no; ++t)
        {
            thread_collection.emplace_back([&,t = t, view = data.view, start = static_cast<size_t>(ranges[t].first), end = static_cast<size_t>(ranges[t].second)]
            {
                std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
                std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

                uint32_t value = 0;
                for (size_t i = start; i < end; ++i)
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
                            value = parse_value_view(view,temp);
                            {
                                auto& [sum, max, min, count] = maps[t].try_emplace(view.substr(city.first, city.second)).first->second;

                                min = std::min(value, min);
                                max = std::max(value, max);
                                sum += value;
                                ++count;

                                city.first = i + 1;
                                ++line_count[t];
                            }
                            break;
                        }
                    default: break;
                    }
                }

                result.total_lines = line_count[t];
            });
        }

        for (auto& t : thread_collection)
        {
            if (t.joinable())
                t.join();
        }

        result.total_lines = std::accumulate(line_count.begin(),line_count.end(),0);
    }
private:
    int _thread_no = 0;
};

#endif //MULTITHREADSPAWNLOCKFREE_H
