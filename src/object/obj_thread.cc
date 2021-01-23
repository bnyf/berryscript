#include "obj_thread.h"

void ObjThread::prepareFrame(ObjClosure* objClosure, Value* stackStart) {
    ASSERT(this->frameCapacity > this->usedFrameNum, "frame not enough!!");   
    //objThread->usedFrameNum是最新可用的frame
    Frame* frame = &(this->frames[this->usedFrameNum++]);

    //thread中的各个frame是共享thread的stack 
    //frame用frame->stackStart指向各自frame在thread->stack中的起始地址
    frame->stackStart = stackStart;
    frame->closure = objClosure;
    frame->ip = objClosure->fn->instrStream.datas;
}

void ObjThread::resetThread(ObjClosure*  objClosure) {
    this->esp = this->stack;  
    this->openUpvalues = NULL;
    this->caller = NULL;
    this->errorObj = INIT_VALUE_OF_TPYE(VT_NULL);
    this->usedFrameNum = 0;

    ASSERT(objClosure != NULL, "objClosure is NULL in function resetThread");
    prepareFrame(objClosure, this->stack);
}

//新建线程
ObjThread::ObjThread(std::shared_ptr<VM> vm, ObjClosure* objClosure) {
    ASSERT(objClosure != NULL, "objClosure is NULL!");

    Frame* frames = ALLOCATE_ARRAY(vm, Frame, INITIAL_FRAME_NUM);

    //加1是为接收者的slot
    uint32_t stackCapacity = ceilToPowerOf2(objClosure->fn->maxStackSlotUsedNum + 1);  
    Value* newStack = ALLOCATE_ARRAY(vm, Value, stackCapacity); 

    ObjThread* objThread = ALLOCATE(vm, ObjThread);
    vm->initAndAddObjHeader(this, OT_THREAD, vm->threadClass);

    objThread->frames = frames;
    objThread->frameCapacity = INITIAL_FRAME_NUM;
    objThread->stack = newStack;
    objThread->stackCapacity = stackCapacity;

    resetThread(objClosure);
}
