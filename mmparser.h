#ifndef PARSER_H
#define PARSER_H
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include "iparser.h"
#include "Strategies/istrategy.h"

class MMParser : public IParser
{
public:
    MMParser(const char* path, std::unique_ptr<IStrategy> strategy): _strategy(std::move(strategy)){
        _fd = open(path,O_RDONLY);

        struct stat st{};
        fstat(_fd, &st);
        _file_size = st.st_size;

        _mm_addr = mmap(nullptr, _file_size, PROT_READ, MAP_PRIVATE, _fd, 0);
    }

    bool start() override;

    ~MMParser() override{
        munmap(_mm_addr, _file_size);
        close(_fd);
    }

private:
    std::unique_ptr<IStrategy> _strategy;
    int _fd = 0;
    size_t _file_size = 0;
    void* _mm_addr;
};

#endif //PARSER_H