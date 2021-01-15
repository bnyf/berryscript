#ifndef _VM_H
#define _VM_H
#include <stdint.h>
#include <memory>
#include "object/obj_header.h"

typedef struct Class_s Class_t;
class Parser;

class VM {
    public:
         VM();
         void* memManager(void* ptr, uint32_t oldSize, uint32_t newSize);
         void initAndAddObjHeader(ObjHeader_t* objHeader, ObjType objType, Class_t* bsClass);
         void incrementAllocatedBytes(uint32_t bytes);
         std::shared_ptr<Parser> curParser;
         Class_t* classOfClass;
         Class_t* objectClass;
         Class_t* mapClass;
         Class_t* rangeClass;
         Class_t* listClass;
         Class_t* nullClass;
         Class_t* boolClass;
         Class_t* numClass;
         Class_t* threadClass;
         
         Class_t* stringClass;
         Class_t* fnClass;
      private:
         ObjHeader_t* allObjects;  //所有已分配对象链表
         uint32_t allocatedBytes; // 已分配的空间
};

#define ALLOCATE(vmPtr, type) \
   (type*)vmPtr->memManager(nullptr, 0, sizeof(type))

#define ALLOCATE_EXTRA(vmPtr, mainType, extraSize) \
   (mainType*)vmPtr->memManager(nullptr, 0, sizeof(mainType) + extraSize)

#define ALLOCATE_ARRAY(vmPtr, type, count) \
   (type*)vmPtr->memManager(nullptr, 0, sizeof(type) * count)

#define DEALLOCATE_ARRAY(vmPtr, arrayPtr, count) \
   vmPtr->memManager(arrayPtr, sizeof(arrayPtr[0]) * count, 0)

#define DEALLOCATE(vmPtr, memPtr) vmPtr->memManager(memPtr, 0, 0)

#endif