#include "vm.h"

VM::VM() {
    curParser = nullptr;
    allocatedBytes = 0;
    allObjects = nullptr;
}

void VM::initAndAddObjHeader(ObjHeader* objHeader, ObjType objType, BerryClass* berryClass) {
    objHeader->type = objType;
    objHeader->isDark = false;
    objHeader->berryClass = berryClass;    //设置meta类
    objHeader->next = allObjects;
    allObjects = objHeader;
}

void* VM::memManager(void* ptr, uint32_t oldSize, uint32_t newSize) {
    allocatedBytes += newSize - oldSize;

    //避免realloc(NULL, 0)定义的新地址,此地址不能被释放
    if (newSize == 0) {
        free(ptr);
        return nullptr;
    }
    
    return realloc(ptr, newSize); 
}

void VM::incrementAllocatedBytes(uint32_t bytes) {
    this->allocatedBytes += bytes;
}