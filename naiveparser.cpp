#include "naiveparser.h"
#include <chrono>
#include <iostream>
#include "constants.h"

bool NaiveParser::start()
{
    const auto start = std::chrono::high_resolution_clock::now();

    if(!_file.is_open())
    {
        std::cerr<<"Failed to open the file\n";
        return false;
    }

    std::string line_buffer;
    line_buffer.reserve(GLOBAL::LINE_LEN);

    std::stringstream ss;
    std::string place_buffer;
    place_buffer.reserve(GLOBAL::LINE_LEN);

    std::string value_buffer;
    value_buffer.reserve(GLOBAL::LINE_LEN);

    uint64_t total_lines = 0;
    double value = 0;

    while(std::getline(_file, line_buffer))
    {
        ++total_lines;
        ss.clear();
        ss.str(line_buffer);
        while(std::getline(ss,place_buffer, GLOBAL::LINE_DELIMITER) && std::getline(ss,value_buffer, GLOBAL::LINE_END))
        {
            auto [ptr, ec] = std::from_chars(value_buffer.data(), value_buffer.data() + value_buffer.size(), value);
            if(ec != std::errc())
            {
                continue;
            }

            auto [max, min, mean, count] = _map.try_emplace(place_buffer,Data()).first->second;
            max = std::max(max, value);
            min = std::min(min, value);
            mean += (value - mean) / ++count;
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "\nTotal lines = " << total_lines << "\n";
    std::cout << "Naive Object method time taken = " << diff.count() << " microseconds\n";
    return true;
}
