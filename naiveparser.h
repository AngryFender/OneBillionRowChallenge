#ifndef NAIVEPARSER_H
#define NAIVEPARSER_H
#include <fstream>
#include <unordered_map>
#include "data.h"
#include "iparser.h"

class NaiveParser final : public IParser {
public:
    explicit NaiveParser(const char* file_path):_file(file_path){}
    bool start() override;
    ~NaiveParser() override = default;
private:
    std::ifstream _file;
    std::unordered_map<std::string, Data> _map;
};

#endif //NAIVEPARSER_H
