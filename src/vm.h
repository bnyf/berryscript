#ifndef _VM_H
#define _VM_H
#include <stdint.h>
#include <memory>

#include "parser.h"

class VM {
public:
    VM();
    void* memManager(void* ptr, uint32_t oldSize, uint32_t newSize);
    std::shared_ptr<Parser> curParser;

private:
    uint32_t allocatedBytes; // 已分配的空间
};

#endif