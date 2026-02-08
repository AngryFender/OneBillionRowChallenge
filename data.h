#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <limits>

constexpr double MIN_VALUE = std::numeric_limits<uint32_t>::lowest();
constexpr double MAX_VALUE = std::numeric_limits<uint32_t>::max();

struct Data {
    uint64_t sum = 0;
    uint32_t max = MIN_VALUE;
    uint32_t min = MAX_VALUE;
    uint32_t count = 0;

    Data(): sum(0), max(MIN_VALUE), min(MAX_VALUE), count(0)
    {
    }

    Data(uint64_t sum, uint32_t max, uint32_t min, uint32_t count)
        : sum(sum),
          max(max),
          min(min),
          count(count)
    {
    }
};


#endif //DATA_H
