#ifndef OBJECT_THREAD_H
#define OBJECT_THREAD_H
#include "obj_fn.h"

class ObjThread : ObjHeader{
    public:
        Value* stack;  //运行时栈的栈底
        Value* esp;    //运行时栈的栈顶
        uint32_t stackCapacity;  //栈容量

        Frame* frames;   //调用框架
        uint32_t usedFrameNum;   //已使用的frame数量
        uint32_t frameCapacity;  //frame容量

        //"打开的upvalue"的链表首结点
        ObjUpvalue* openUpvalues;

        //当前thread的调用者
        ObjThread* caller;

        //导致运行时错误的对象会放在此处,否则为空
        Value errorObj;

        ObjThread(std::shared_ptr<VM> vm, ObjClosure* objClosure);
        void prepareFrame(ObjClosure* objClosure, Value* stackStart);
        void resetThread(ObjClosure*  objClosure);
};    //线程对象

#endif //BERRYSCRIPT_OBJ_THREAD_H
