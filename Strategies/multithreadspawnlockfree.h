#ifndef MULTITHREADSPAWNLOCKFREE_H
#define MULTITHREADSPAWNLOCKFREE_H

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

class MultiThreadSpawnLockFree final: public IStrategy
{
public:
    explicit MultiThreadSpawnLockFree(const int num_of_threads = 0, const size_t chunk_size = 0):_thread_no(num_of_threads), _chunk_size(chunk_size){}
    ~MultiThreadSpawnLockFree()  override = default;

    void parse(const StratInfo& data, StratResult& result) override
    {
        result.name.append(std::to_string(_thread_no) + " Threads Spawn "+std::to_string(_chunk_size)+" chunk_size"+" for MultiThreadSpawnLockFree ");

        using MapData = emhash7::HashMap<std::string_view, Data>;
        std::vector<MapData> map;

        size_t total_chunk_count = 0;
        std::vector<std::pair<size_t, size_t>> chunks;
        size_t lower = 0;
        size_t higher = 0;

        while (lower < data.file_size)
        {
            higher = find_eol(data.view, std::min(data.file_size, higher + _chunk_size), data.file_size);
            chunks.emplace_back(lower,higher);
            lower = higher + 1;
        }
        total_chunk_count = chunks.size();

        std::vector<std::thread> thread_collection;
        thread_collection.reserve(_thread_no);
        for (int t = 0; t < _thread_no; ++t)
        {
            map.emplace_back();
            map.back().reserve(2000);
            map.back().max_load_factor(0.7f);
        }

        std::atomic_int32_t atomic_chunk_tracker{0};
        std::vector<uint64_t> line_count(_thread_no,uint64_t{0});
        for (int t = 0; t < _thread_no; ++t)
        {
            thread_collection.emplace_back([&,t = t, view = data.view]
            {
                std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
                std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

                int32_t value = 0;
                uint32_t thread_chunk = atomic_chunk_tracker++;

                while (thread_chunk < total_chunk_count)
                {
                    const size_t start = chunks[thread_chunk].first;
                    const size_t end = chunks[thread_chunk].second;
                    city.first = start;

                    for (size_t i = start; i <= end; ++i)
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
                                    auto& [sum, max, min, count] = map[t][view.substr(city.first,city.second)];
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
                    thread_chunk = atomic_chunk_tracker++;
                }
            });
        }

        for (auto& t : thread_collection)
        {
            if (t.joinable())
                t.join();
        }

        result.total_lines = std::accumulate(line_count.begin(),line_count.end(),uint64_t{0});
    }
private:
    int _thread_no = 0;
    size_t _chunk_size = 0;
};

#endif //MULTITHREADSPAWNLOCKFREE_H
