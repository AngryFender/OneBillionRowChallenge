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

#include "helper.h"
#include "naiveparser.h"
#include "mmparser.h"
#include "Strategies/multithreadspawn.h"
#include "Strategies/parentthread.h"
#include "Strategies/singlethreadspawn.h"

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

    uint32_t value = 0;

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

    for (int t = 1; t <= 32; t = t * 2)
    {
        MMParser multi_thread_mm_parser(DATA_FILE_PATH, std::make_unique<MultiThreadSpawn>(t));
        multi_thread_mm_parser.start();
    }

    auto start_time_mm_fly = std::chrono::high_resolution_clock::now();
    mmap_flyweight_method();
    auto end_time_mm_fly = std::chrono::high_resolution_clock::now();
    auto diff_time_mm_fly = std::chrono::duration_cast<std::chrono::microseconds>(end_time_mm_fly - start_time_mm_fly).count();
    std::cout << "Memory Mapping Flyweight Time taken = " << diff_time_mm_fly << " microseconds\n";

    return 0;
}

