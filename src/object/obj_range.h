#ifndef _OBJECT_RANGE_H
#define _OBJECT_RANGE_H


#include <memory>

#include "obj_class.h"
#include "../vm.h"

class ObjRange : public ObjHeader{
    public:
        ObjHeader objHeader;
        int from;   //范围的起始
        int to;     //范围的结束
        ObjRange(std::shared_ptr<VM> vm, uint32_t from, uint32_t to);
};  //range对象


#endif
