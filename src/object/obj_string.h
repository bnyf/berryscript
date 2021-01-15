#ifndef _OBJECT_STRING_H
#define _OBJECT_STRING_H

#include "../utils.h"
#include "../vm.h"
#include "obj_header.h"


typedef struct String_s{
   uint32_t length;
   char* str;
} String_t;

class ObjString{
    public:
        ObjHeader_t objHeader;
        uint32_t hashCode;  //字符串的哈希值
        String_t value;
        
        ObjString(std::shared_ptr<VM> vm, const char* str = "", size_t length = 0);
        uint32_t hashString(char* str, uint32_t length);
        void hashMyself();
};

#endif
