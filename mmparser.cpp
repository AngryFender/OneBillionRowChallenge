#include "mmparser.h"
#include <iostream>
#include <chrono>

bool MMParser::start()
{
    if (_mm_addr == reinterpret_cast<void*>(-1))
    {
        std::cerr << "failed registering mmap of the file: " << strerror(errno) << "\n";
        return false;
    }

    const auto start_time = std::chrono::high_resolution_clock::now();

    const char* begin = static_cast<char*>(_mm_addr);
    const char* end = begin + _file_size;
    const StratInfo info{.view = {begin, end}, .file_size = _file_size};
    StratResult result;
    _strategy->parse(info, result);

    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto diff_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    const double billion_time = 1000000000/static_cast<double>(result.total_lines) * (static_cast<double>(diff_time.count())/1000000);

    std::cout << "\nTotal lines = " << result.total_lines << "\n";
    std::cout << "Memory map : " << result.name << " time taken = " << diff_time.count() << " microseconds | Billion row time = "<<billion_time<<" seconds\n";

    return true;
}
