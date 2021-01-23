#include "obj_fn.h"

ObjFn::ObjFn(std::shared_ptr<VM> vm, ObjModule* objModule, uint32_t slotNum) : instrStream(vm), constants(vm) {
    vm->initAndAddObjHeader(this, OT_FUNCTION, vm->fnClass);
    this->module = objModule;
    this->maxStackSlotUsedNum = slotNum;
    this->upvalueNum = this->argNum = 0;
}

ObjUpvalue::ObjUpvalue(std::shared_ptr<VM> vm, Value* localVarPtr) {
    vm->initAndAddObjHeader(this, OT_UPVALUE, NULL);
    this->localVarPtr = localVarPtr;
    this->next = NULL;
}

ObjClosure::ObjClosure(std::shared_ptr<VM> vm, ObjFn* objFn) {
    this->fn = objFn;
    this->upvalues = ALLOCATE_ARRAY(vm, ObjUpvalue*, this->fn->upvalueNum);
    vm->initAndAddObjHeader(this, OT_CLOSURE, vm->fnClass);
    
    //清除upvalue数组做 以避免在填充upvalue数组之前触发GC
    for(int i=0;i<this->fn->upvalueNum;++i){
        this->upvalues[i] = NULL; 
    }
}