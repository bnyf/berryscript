#ifndef _OBJECT_LIST_H
#define _OBJECT_LIST_H
#include "obj_class.h"
#include "../vm.h"

class ObjList : public ObjHeader{
    public:
        ObjList(std::shared_ptr<VM> vm, uint32_t elementNum);
        Value removeElement(uint32_t index);
        void insertElement(uint32_t index, Value value);
    private:
        std::shared_ptr<VM> vm;
        DataBuffer<Value>elements;  //list中的元素
        void shrinkList(uint32_t newCapacity);
};  //list对象

#endif
