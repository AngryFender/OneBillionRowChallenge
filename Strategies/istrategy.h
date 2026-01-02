#ifndef ISTRATEGY_H
#define ISTRATEGY_H

struct StratInfo
{
    std::string_view view;
    size_t file_size = 0;
};

struct StratResult
{
    std::string name{};
    uint64_t total_lines = 0;
};

class IStrategy
{
public:
    virtual ~IStrategy() = default;
    virtual void parse(const StratInfo& data, StratResult& result) = 0;
};

#endif //ISTRATEGY_H
