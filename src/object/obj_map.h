#ifndef _OBJECT_MAP_H
#define _OBJECT_MAP_H

#include "header.h"
#include "obj_class.h"
#include "../vm.h"

#define MAP_LOAD_PERCENT 0.8

struct Entry{  
    Value key; 
    Value value;
};   //key->value对儿

class ObjMap : public ObjHeader{
    public:
        ObjMap(std::shared_ptr<VM> vm);
        bool mapSet(Value key, Value value);
        Value mapGet(Value key);
        void clearMap();
        Value removeKey(Value key);
    private:
        uint32_t capacity; //Entry的容量(即总数),包括已使用和未使用Entry的数量
        uint32_t count;  //map中使用的Entry的数量
        Entry* entries; //Entry数组
        std::shared_ptr<VM> vm;
        Entry* findEntry(Value key);
        void resizeMap(uint32_t newCapacity);
};


#endif
