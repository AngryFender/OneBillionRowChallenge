#ifndef PARSER_H
#define PARSER_H
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include "iparser.h"
#include "Strategies/istrategy.h"

class MMParser : public IParser
{
public:
    MMParser(const char* path, std::unique_ptr<IStrategy> strategy): _strategy(std::move(strategy)){
        _fd = open(path,O_RDONLY);

        struct stat st{};
        fstat(_fd, &st);
        _size = st.st_size;

        _addr = mmap(nullptr, _size, PROT_READ, MAP_PRIVATE, _fd, 0);
        std::cout << "\nthe size of the file = " << std::to_string(_size) << "\n";
    }

    bool start() override
    {
        if (_addr == reinterpret_cast<void*>(-1))
        {
            std::cerr << "failed registering mmap of the file: " << strerror(errno) << "\n";
            return false;
        }

        const char* begin = static_cast<char*>(_addr);
        const char* end = begin + _size;
        const std::string_view view(begin, end);
        _strategy->parse(view);

        return true;
    }

    ~MMParser() override{
        munmap(_addr, _size);
        close(_fd);
    }

private:
    std::unique_ptr<IStrategy> _strategy;
    int _fd = NULL;
    size_t _size = 0;
    void* _addr;
};

#endif //PARSER_H