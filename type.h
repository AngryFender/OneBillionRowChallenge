#ifndef TYPE_H
#define TYPE_H
#include <cstdint>
#include <string_view>

struct Key
{
    uint64_t hash;
    std::string_view view;
};

#endif //TYPE_H
