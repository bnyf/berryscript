#ifndef _FN_H
#define _FN_H

#include "../utils.h"
#include "../dataBuffer.h"
#include "../vm.h"
#include "obj_class.h"
#include "header.h"
#include <vector>

typedef struct FnDebug_s{
    std::string fnName;
    std::vector<int> lineNo;
} FnDebug_t;

class ObjFn : public ObjHeader{
    public:
        DataBuffer<uint8_t> instrStream;  //函数编译后的指令流
        DataBuffer<Value> constants;   // 函数中的常量表

        ObjModule* module;    //本函数所属的模块

        //本函数最多需要的栈空间,是栈使用空间的峰值
        uint32_t maxStackSlotUsedNum;
        uint32_t upvalueNum;	  //本函数所涵盖的upvalue数量
        uint8_t argNum;   //函数期望的参数个数
    #if DEBUG
        FnDebug_t* debug;
    #endif
        ObjFn(std::shared_ptr<VM> vm, ObjModule* objModule, uint32_t slotNum);

}; //函数对象, 所有指令流单元都用 objfn 表示

class ObjUpvalue : public ObjHeader {
    //栈是个Value类型的数组,localVarPtr指向upvalue所关联的局部变量
    Value* localVarPtr;

    //存放已被关闭的localVarPtr指向的upvalue，localVarPtr也会变为指向该变量
    Value closedUpvalue;

    ObjUpvalue* next;   //用以链接openUpvalue链表

    ObjUpvalue(std::shared_ptr<VM> vm, Value* localVarPtr);
}; //upvalue对象

class ObjClosure : public ObjHeader{
    public:
        ObjFn* fn;   //闭包中所要引用的函数
        ObjUpvalue** upvalues; //用于存储此函数的 "close upvalue"
        ObjClosure(std::shared_ptr<VM> vm, ObjFn* objFn);
}; //闭包对象

struct Frame{
   uint8_t* ip;    //程序计数器,指向下一个将被执行的指令

   //在本frame中执行的闭包函数
   ObjClosure* closure;
   
   //frame是共享therad.stack
   //此项用于指向本frame所在thread运行时栈的起始地址
   Value* stackStart;
};  //调用框架

#define INITIAL_FRAME_NUM 4

#endif