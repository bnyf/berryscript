#ifndef _OBJECT_STRING_H
#define _OBJECT_STRING_H

#include "../utils.h"
#include "../vm.h"
#include "header.h"

struct BerryString{
   uint32_t length;
   char* str;
};

class ObjString : public ObjHeader {
    public:
        ObjString(std::shared_ptr<VM> vm, const char* str = "", size_t length = 0);
        void hashMyself();
        BerryString value;
        uint32_t hashCode;  //字符串的哈希值
};

#endif
