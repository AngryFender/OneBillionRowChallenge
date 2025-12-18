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
#include <boost/flyweight.hpp>

constexpr int LINE_SIZE = 30;
constexpr char DELIMITER = ';';
constexpr double MIN_DOUBLE = std::numeric_limits<double>::lowest();
constexpr double MAX_DOUBLE = std::numeric_limits<double>::max();

struct Data {
    double max = MIN_DOUBLE;
    double min = MAX_DOUBLE;
    double mean = 0;
    size_t count = 0;
};

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

void naive_method()
{
    std::ifstream file(DATA_FILE_PATH);
    if (!file.is_open())
    {
        std::cout << "File didn't open\n";
    }
    int total = 0;
    std::unordered_map<std::string, Data> map;
    std::string line_buffer;
    line_buffer.reserve(LINE_SIZE);
    std::stringstream ss(line_buffer);
    std::string place_buffer;
    std::string value_buffer;
    double v = 0;
    while (std::getline(file, line_buffer))
    {
        v = 0;
        ss.clear();
        ss.str(line_buffer);
        if (std::getline(ss, place_buffer, DELIMITER) && std::getline(ss, value_buffer, DELIMITER))
        {
            try
            {
                v = std::stod(value_buffer);
                auto& data = map.try_emplace(place_buffer).first->second;
                data.min = std::min(v, data.min);
                data.max = std::max(v, data.max);
                data.mean += (v - data.mean) / ++data.count;
            }
            catch (std::exception& ex)
            {
            }
        }
        ++total;
    };

    //for (const auto& [key,data] : map) {
    //   std::cout << key << "," << data.min << "," << data.mean << "," << data.max << "," << data.count << "\n";
    //}
    std::cout << "\nTotal rows = " << total << "\n";
}



int main() {

    auto start_time = std::chrono::high_resolution_clock::now();
    naive_method();
    auto end_time = std::chrono::high_resolution_clock::now();
	auto diff_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Naive Approach Time taken = " << diff_time << " milliseconds\n";

    auto start_time_mm = std::chrono::high_resolution_clock::now();
    mmap_method();
    auto end_time_mm = std::chrono::high_resolution_clock::now();
    auto diff_time_mm = std::chrono::duration_cast<std::chrono::microseconds>(end_time_mm - start_time_mm).count();
    std::cout << "Memory Mapping Time taken = " << diff_time_mm << " microseconds\n";

    auto start_time_mm_fly = std::chrono::high_resolution_clock::now();
    mmap_flyweight_method();
    auto end_time_mm_fly = std::chrono::high_resolution_clock::now();
    auto diff_time_mm_fly = std::chrono::duration_cast<std::chrono::microseconds>(end_time_mm_fly - start_time_mm_fly).count();
    std::cout << "Memory Mapping Flyweight Time taken = " << diff_time_mm_fly << " microseconds\n";

    return 0;
}
