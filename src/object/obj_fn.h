#ifndef _OBJECT_FN_H
#define _OBJECT_FN_H

#include "../utils.h"

#include "obj_meta.h"
#include <vector>

typedef struct Value_s Value_t;

typedef struct FnDebug_s{
    std::string fnName;
    std::vector<int> lineNo;
} FnDebug_t;


typedef struct ObjFn_s{
    ObjHeader_t objHeader;
    DataBuffer<uint8_t> instrStream;  //函数编译后的指令流
    DataBuffer<Value_t> constants;   // 函数中的常量表

    ObjModule_t* module;    //本函数所属的模块

    //本函数最多需要的栈空间,是栈使用空间的峰值
    uint32_t maxStackSlotUsedNum;
    uint32_t upvalueNum;	  //本函数所涵盖的upvalue数量
    uint8_t argNum;   //函数期望的参数个数
#if DEBUG
    FnDebug_t* debug;
#endif

    ObjFn_s(std::shared_ptr<VM> vm, ObjModule_t* objModule, uint32_t slotNum) : instrStream(vm), constants(vm) {
        vm->initAndAddObjHeader(&this->objHeader, OT_FUNCTION, vm->fnClass);
        this->module = objModule;
        this->maxStackSlotUsedNum = slotNum;
        this->upvalueNum = this->argNum = 0;
    }

}ObjFn_t; //函数对象, 所有指令流单元都用 objfn 表示

typedef struct ObjUpvalue_s {
    ObjHeader_t objHeader;

    //栈是个Value类型的数组,localVarPtr指向upvalue所关联的局部变量
    Value_t* localVarPtr;

    //已被关闭的upvalue
    Value_t closedUpvalue;

    ObjUpvalue_s* next;   //用以链接openUpvalue链表

    ObjUpvalue_s(std::shared_ptr<VM> vm, Value_t* localVarPtr) {
        vm->initAndAddObjHeader(&this->objHeader, OT_UPVALUE, NULL);
        this->localVarPtr = localVarPtr;
        this->next = NULL;
    }
}ObjUpvalue_t; //upvalue对象

typedef struct ObjClosure_s {
    ObjHeader_t objHeader;
    ObjFn_t* fn;   //闭包中所要引用的函数
    ObjUpvalue_t** upvalues; //用于存储此函数的 "close upvalue"

    ObjClosure_s(std::shared_ptr<VM> vm, ObjFn_t* objFn) {
        this->fn = objFn;
        this->upvalues = ALLOCATE_ARRAY(vm, ObjUpvalue_t*, this->fn->upvalueNum);
        vm->initAndAddObjHeader(&this->objHeader, OT_CLOSURE, vm->fnClass);
        
        //清除upvalue数组做 以避免在填充upvalue数组之前触发GC
        for(int i=0;i<this->fn->upvalueNum;++i){
            this->upvalues[i] = NULL; 
        }
    }
}ObjClosure_t; //闭包对象

typedef struct {
   uint8_t* ip;    //程序计数器,指向下一个将被执行的指令

   //在本frame中执行的闭包函数
   ObjClosure_t* closure;
   
   //frame是共享therad.stack
   //此项用于指向本frame所在thread运行时栈的起始地址
   Value_t* stackStart;
} Frame;  //调用框架

#endif