#ifndef DATA_H
#define DATA_H
#include <limits>

constexpr double MIN_DOUBLE = std::numeric_limits<double>::lowest();
constexpr double MAX_DOUBLE = std::numeric_limits<double>::max();

struct Data {
    double max = MIN_DOUBLE;
    double min = MAX_DOUBLE;
    double mean = 0;
    size_t count = 0;
};


#endif //DATA_H
