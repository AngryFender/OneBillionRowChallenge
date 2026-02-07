#ifndef MULTITHREADSPAWNLOCKFREEDOUBLEMAP_H
#define MULTITHREADSPAWNLOCKFREEDOUBLEMAP_H

#include <numeric>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>
#include "../type.h"

#include "../helper.h"
#include "../data.h"
#include "istrategy.h"
#include "hash_table7.hpp"

class MultiThreadSpawnLockFreeDoubleMap final: public IStrategy
{
public:
    explicit MultiThreadSpawnLockFreeDoubleMap(const int num_of_threads = 0, const size_t chunk_size = 0):_thread_no(num_of_threads), _chunk_size(chunk_size){}
    ~MultiThreadSpawnLockFreeDoubleMap()  override = default;

    void parse(const StratInfo& data, StratResult& result) override
    {
        result.name.append(std::to_string(_thread_no) + " Threads Spawn "+std::to_string(_chunk_size)+" chunk_size"+" for MultiThreadSpawnLockFree ");

        using MapData = emhash7::HashMap<uint32_t, Data>;
        using MapString = emhash7::HashMap<std::string_view,uint32_t>;
        std::vector<std::thread> thread_collection;
        thread_collection.reserve(_thread_no);

        std::vector<MapString> maps_key;
        std::vector<MapData> maps_data;

        size_t chunk_num = data.file_size/ _chunk_size;
        if(chunk_num * _chunk_size < data.file_size)
        {
            ++chunk_num;
        }

        std::vector<std::pair<size_t, size_t>> chunks;
        chunks.reserve(chunk_num);

        size_t lower = 0;
        size_t higher = 0;
        while (lower < data.file_size)
        {
            higher = find_eol_reverse(data.view, std::min(data.file_size, higher + _chunk_size), data.file_size);
            chunks.emplace_back(lower,higher);
            lower = higher+1;
        }

        const float factor = static_cast<float>(data.file_size) / static_cast<float>(_thread_no);

        std::vector<std::pair<float, float>> ranges;
        ranges.reserve(_thread_no);

        float high = 0;
        size_t curr = 0;
        size_t prev = 0;

        for (int t = 0; t < _thread_no; ++t)
        {
            curr = find_eol_reverse(data.view, static_cast<size_t>(std::min(++high * factor, static_cast<float>(data.file_size))), data.file_size);
            ranges.emplace_back(prev, curr);
            prev = ++curr;

            maps_key.emplace_back();
            maps_key.back().reserve(10000);
            maps_key.back().max_load_factor(0.7f);

            maps_data.emplace_back();
            maps_data.back().reserve(10000);
            maps_data.back().max_load_factor(0.7f);
        }

        std::atomic_int32_t chunk_tracker{0};
        std::vector<uint64_t> line_count(_thread_no,0);
        for (int t = 0; t < _thread_no; ++t)
        {
            thread_collection.emplace_back([&,t = t, view = data.view]
            {
                std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
                std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

                uint32_t value = 0;
                uint32_t chunk_current = chunk_tracker++;
                std::string_view city_view;
                uint32_t id = 0;
                uint32_t current_id = 0;

                while (chunk_current <= chunk_num)
                {
                    const size_t start = chunks[chunk_current].first;
                    const size_t end = chunks[chunk_current].second;
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
                                value = parse_value_view(view, temp);
                                {
                                    // city_view = view.substr(city.first, city.second);
                                    current_id = maps_key[t][view.substr(city.first, city.second)];

                                    auto& [sum, max, min, count] = maps_data[t][current_id];
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
                    chunk_current = chunk_tracker++;
                }
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
    size_t _chunk_size = 0;
};

#endif //MULTITHREADSPAWNLOCKFREEDOUBLEMAP_H
