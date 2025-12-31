#ifndef ISTRATEGY_H
#define ISTRATEGY_H

class IStrategy
{
public:
    virtual ~IStrategy() = default;
    virtual void parse(std::string_view view) = 0;
};

#endif //ISTRATEGY_H
