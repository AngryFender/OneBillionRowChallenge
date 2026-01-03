#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <limits>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <charconv>
#include <thread>
#include <boost/flyweight.hpp>

#include "naiveparser.h"
#include "mmparser.h"
#include "Strategies/parentthread.h"
#include "Strategies/singlethreadspawn.h"

constexpr int LINE_SIZE = 30;
constexpr char DELIMITER = ';';

void do_work_one_thread(std::string_view view, const size_t start, const size_t end, std::unordered_map<std::string_view, Data>& map)
{
    std::string_view value_view;
    double value = 0;
    std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
    std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

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
                // parse float using from_chars -> value
                value_view = view.substr(temp.first, i - temp.first);
                auto [ptr, ec] = std::from_chars(value_view.data(), value_view.data() + value_view.size(),value);

                    auto& [max, min,mean, count] = map.try_emplace(view.substr(city.first, city.second)).first->second;

                    min = std::min(value, min);
                    max = std::max(value, max);
                    mean += (value - mean) / static_cast<double>(++count);

                city.first = i + 1;
                break;
            }
        default: break;
        }
    }
}
void mmap_method()
{
    int fd = open(DATA_FILE_PATH,O_RDONLY);

    struct stat st;
    fstat(fd,&st);
    size_t size = st.st_size;

    std::cout << "\nthe size of the file = " << std::to_string(size) << "\n";
    void* addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(addr == reinterpret_cast<void*>(-1))
    {
        std::cerr << "failed registering mmap of the file: " << strerror(errno) << "\n";
        return;
    }

    char *begin = static_cast<char*>(addr);
    char *end = begin + size;
    std::string_view view(begin, end);


    std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
    std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

    std::unordered_map<std::string_view, Data> map;

    std::string_view value_view;
    double value = 0;

    for(size_t i = 0; i < size; ++i)
    {
        switch(view[i])
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

                // parse float using from_chars -> value
                value_view = view.substr(temp.first, i-temp.first);
                auto [ptr, ec] = std::from_chars(value_view.data(), value_view.data()+value_view.size(), value);

                min = std::min(value, min);
                max = std::max(value, max);
                mean += (value - mean) / static_cast<double>(++count);

                city.first = i + 1;
                break;
            }
            default:break;
        }
    }

    // cleanup
    munmap(addr,size);
    close(fd);
}

void do_work(std::string_view view, const size_t start, const size_t end, std::unordered_map<std::string_view, Data>& map, std::mutex& mutex)
{
    std::string_view value_view;
    double value = 0;
    std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
    std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

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
                // parse float using from_chars -> value
                value_view = view.substr(temp.first, i - temp.first);
                auto [ptr, ec] = std::from_chars(value_view.data(), value_view.data() + value_view.size(),value);

                {
                    std::lock_guard lock(mutex);
                    auto& [max, min,mean, count] = map.try_emplace(view.substr(city.first, city.second)).first->second;

                    min = std::min(value, min);
                    max = std::max(value, max);
                    mean += (value - mean) / static_cast<double>(++count);
                }

                city.first = i + 1;
                break;
            }
        default: break;
        }
    }
}
std::unordered_map<std::string_view, Data> do_work_independently(std::string_view view, const size_t start, const size_t end)
{
    std::string_view value_view;
    double value = 0;
    std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
    std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first
    std::unordered_map<std::string_view, Data> map;

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
                // parse float using from_chars -> value
                value_view = view.substr(temp.first, i - temp.first);
                auto [ptr, ec] = std::from_chars(value_view.data(), value_view.data() + value_view.size(), value);

                auto& [max, min,mean, count] = map.try_emplace(view.substr(city.first, city.second)).first->second;

                min = std::min(value, min);
                max = std::max(value, max);
                mean += (value - mean) / static_cast<double>(++count);

                city.first = i + 1;
                break;
            }
        default: break;
        }
    }
    return map;
}

void mmap_with_one_spawn_thread_method()
{
    int fd = open(DATA_FILE_PATH,O_RDONLY);

    struct stat st;
    fstat(fd,&st);
    size_t size = st.st_size;

    std::cout << "\nthe size of the file = " << std::to_string(size) << "\n";
    void* addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(addr == reinterpret_cast<void*>(-1))
    {
        std::cerr << "failed registering mmap of the file: " << strerror(errno) << "\n";
        return;
    }

    char *begin = static_cast<char*>(addr);
    char *end = begin + size;
    std::string_view view(begin, end);

    std::unordered_map<std::string_view, Data> map;

    std::mutex mutex;
    constexpr int thread_total = 1;
    std::vector<std::thread> thread_collection;
    thread_collection.reserve(thread_total);

    int64_t high = 0;
    int64_t low = -1;
    const size_t factor = size / thread_total;

    std::vector<std::pair<size_t, size_t>> ranges;
    ranges.reserve(thread_total);
    for(int t = 0; t < thread_total;++t)
    {
        ranges.emplace_back(++low * factor, std::min(++high * factor, size));
    }

    for (int t = 0; t < thread_total; ++t)
    {
        thread_collection.emplace_back([&,view](){
            do_work_one_thread(view, 0, size,map);
        });
    }

    for(auto& t : thread_collection)
    {
        if(t.joinable())
            t.join();
    }

    // cleanup
    munmap(addr,size);
    close(fd);
}

void mmap_with_multi_thread_method()
{
    int fd = open(DATA_FILE_PATH,O_RDONLY);

    struct stat st;
    fstat(fd,&st);
    size_t size = st.st_size;

    std::cout << "\nthe size of the file = " << std::to_string(size) << "\n";
    void* addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(addr == reinterpret_cast<void*>(-1))
    {
        std::cerr << "failed registering mmap of the file: " << strerror(errno) << "\n";
        return;
    }

    char *begin = static_cast<char*>(addr);
    char *end = begin + size;
    std::string_view view(begin, end);

    std::unordered_map<std::string_view, Data> map;

    std::mutex mutex;
    constexpr int thread_total = 16;
    std::vector<std::thread> thread_collection;
    thread_collection.reserve(thread_total);

    int64_t high = 0;
    int64_t low = -1;
    const size_t factor = size / thread_total;

    std::vector<std::pair<size_t, size_t>> ranges;
    ranges.reserve(thread_total);
    for(int t = 0; t < thread_total;++t)
    {
        ranges.emplace_back(++low * factor, std::min(++high * factor, size));
    }
    
    for (int t = 0; t < thread_total; ++t)
    {
        thread_collection.emplace_back([&,view](){
            do_work_independently(view,ranges[t].first,std::min( ranges[t].second, size));
        });
    }

    for(auto& t : thread_collection)
    {
        if(t.joinable())
            t.join();
    }

    // cleanup
    munmap(addr,size);
    close(fd);
}

void mmap_flyweight_method()
{
    int fd = open(DATA_FILE_PATH,O_RDONLY);

    struct stat st;
    fstat(fd,&st);
    size_t size = st.st_size;

    std::cout << "\nthe size of the file = " << std::to_string(size) << "\n";
    void* addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(addr == reinterpret_cast<void*>(-1))
    {
        std::cerr << "failed registering mmap of the file: " << strerror(errno) << "\n";
        return;
    }

    char *begin = static_cast<char*>(addr);
    char *end = begin + size;
    std::string_view view(begin, end);

    std::pair<size_t, size_t> city{0, 0}; //first = starting pos, second = count of characters after first
    std::pair<size_t, size_t> temp{0, 0}; //first = starting pos, second = count of characters after first

    using Location = boost::flyweight<std::string>;
    std::unordered_map<Location, Data> map;

    std::string_view value_view;
    double value = 0;


    for(size_t i = 0; i < size; ++i)
    {
        switch(view[i])
        {
        case ';':
            {
                city.second = i - city.first;
                temp.first = i + 1;
                break;
            }
        case '\n':
            {
                std::string_view location_view = view.substr(city.first, city.second);
                Location fly(location_view);
                auto& [max, min,mean, count] = map.try_emplace(fly).first->second;

                // parse float using from_chars -> value
                value_view = view.substr(temp.first, i-temp.first);
                auto [ptr, ec] = std::from_chars(value_view.data(), value_view.data()+value_view.size(), value);

                min = std::min(value, min);
                max = std::max(value, max);
                mean += (value - mean) / static_cast<double>(++count);

                city.first = i + 1;
                break;
            }
            default:break;
        }
    }

    // cleanup
    munmap(addr,size);
    close(fd);
}

int main() {

    NaiveParser naive(DATA_FILE_PATH);
    naive.start();

    {
        MMParser parent_thread_mm_parser(DATA_FILE_PATH, std::make_unique<ParentThread>());
        parent_thread_mm_parser.start();
    }

    {
        MMParser single_thread_mm_parser(DATA_FILE_PATH, std::make_unique<SingleThreadSpawn>());
        single_thread_mm_parser.start();
    }

    auto start_time_mm = std::chrono::high_resolution_clock::now();
    mmap_method();
    auto end_time_mm = std::chrono::high_resolution_clock::now();
    auto diff_time_mm = std::chrono::duration_cast<std::chrono::microseconds>(end_time_mm - start_time_mm).count();
    std::cout << "Memory Mapping Time taken = " << diff_time_mm << " microseconds\n";

    auto start_time_mm_st = std::chrono::high_resolution_clock::now();
    mmap_with_one_spawn_thread_method();
    auto end_time_mm_st = std::chrono::high_resolution_clock::now();
    auto diff_time_mm_st = std::chrono::duration_cast<std::chrono::microseconds>(end_time_mm_st - start_time_mm_st).count();
    std::cout << "Memory Mapping One spawn thread Time taken = " << diff_time_mm_st << " microseconds\n";

    auto start_time_mm_mt = std::chrono::high_resolution_clock::now();
    mmap_with_multi_thread_method();
    auto end_time_mm_mt = std::chrono::high_resolution_clock::now();
    auto diff_time_mm_mt = std::chrono::duration_cast<std::chrono::microseconds>(end_time_mm_mt - start_time_mm_mt).count();
    std::cout << "Memory Mapping MultiThreading Time taken = " << diff_time_mm_mt << " microseconds\n";

    auto start_time_mm_fly = std::chrono::high_resolution_clock::now();
    mmap_flyweight_method();
    auto end_time_mm_fly = std::chrono::high_resolution_clock::now();
    auto diff_time_mm_fly = std::chrono::duration_cast<std::chrono::microseconds>(end_time_mm_fly - start_time_mm_fly).count();
    std::cout << "Memory Mapping Flyweight Time taken = " << diff_time_mm_fly << " microseconds\n";

    return 0;
}

