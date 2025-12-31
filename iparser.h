#ifndef IPARSER_H
#define IPARSER_H

class IParser
{
public:
    virtual ~IParser() = default;
    virtual bool start() = 0;
};

#endif //IPARSER_H
