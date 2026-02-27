#ifndef DATA_H
#define DATA_H
#include <cstdint>
#include <limits>

constexpr double MIN_VALUE = std::numeric_limits<int32_t>::lowest();
constexpr double MAX_VALUE = std::numeric_limits<int32_t>::max();

struct Data {
    int64_t sum = 0;
    int32_t max = MIN_VALUE;
    int32_t min = MAX_VALUE;
    uint32_t count = 0;

    Data() = default;

    Data(const uint64_t sum, const uint32_t max, const uint32_t min, const uint32_t count)
        : sum(sum),
          max(max),
          min(min),
          count(count)
    {
    }
};


#endif //DATA_H
