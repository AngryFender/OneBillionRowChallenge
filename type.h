#ifndef TYPE_H
#define TYPE_H
#include <cstdint>
#include <string_view>
#include <unordered_map>

#include "data.h"

struct Key
{
    uint64_t hash;
    std::string_view view;
};

struct KeyHash
{
    size_t operator()(const Key& k ) const{
        return k.hash;
    }
};

struct KeyEq
{
    bool operator()(const Key& left, const Key& right) const noexcept
    {
        return left.hash == right.hash && left.view == right.view;
    }
};

std::unordered_map<Key, Data, KeyHash, KeyEq> map;

struct String_view_hash
{
    using is_transparent = void;
    size_t operator()(std::string_view sv) const
    {
        return std::hash<std::string_view>{}(sv);
    }

    size_t operator()(std::string& s) const
    {
        return std::hash<std::string>{}(s);
    }

};

using MapKey = std::unordered_map<std::string, uint32_t, String_view_hash, std::equal_to<>>;

#endif //TYPE_H
