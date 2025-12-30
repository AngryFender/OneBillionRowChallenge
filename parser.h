#ifndef PARSER_H
#define PARSER_H
#include <memory>
#include "iparser.h"
#include "Strategies/istrategy.h"

class Parser : public IParser
{
public:
    Parser(const char* path, std::unique_ptr<IStrategy> strat): _strat(std::move(strat)){
    }

    ~Parser(){
    }

private:
    std::unique_ptr<IStrategy> _strat;
};

#endif //PARSER_H
