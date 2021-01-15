#ifndef _OBJECT_META_H
#define _OBJECT_META_H

#include "obj_string.h"
#include "../dataBuffer.h"
#include "../vm.h"
#include "class.h"
#include <memory>

typedef struct Value_s Value_t;

typedef struct ObjModule_s{
    ObjHeader_t objHeader;
    DataBuffer<String_t> moduleVarName;   //模块中的模块变量名
    DataBuffer<Value_t> moduleVarValue;  //模块中的模块变量值
    ObjString name;   //模块名
    ObjModule_s(std::shared_ptr<VM>vm, const char* modName) : moduleVarName(vm), moduleVarValue(vm), name(vm) {
        //ObjModule是元信息对象,不属于任何一个类
        vm->initAndAddObjHeader(&this->objHeader, OT_MODULE, NULL);
        if (modName != NULL) {
            this->name = ObjString(vm, modName, strlen(modName));
        }
    }
}ObjModule_t;   //模块对象

typedef struct ObjInstance_s{
    ObjHeader_t objHeader;
    //具体的字段
    int fieldNum;
    Value_t *fields;
    ObjInstance_s(VM* vm, Class_t* bsClass) {
        this->fieldNum = bsClass->fieldNum;
        //参数class主要作用是提供类中field的数目
        this->fields = ALLOCATE_ARRAY(vm, Value_t, this->fieldNum);
        //初始化field为NULL
        for(int i=0;i<fieldNum;++i) {
            (this->fields)[i] = INIT_VALUE_OF_TPYE(VT_NULL);
        }

        //在此关联对象的类为参数class
        vm->initAndAddObjHeader(&this->objHeader, OT_INSTANCE, bsClass);
    }

}ObjInstance_t;	//对象实例

#endif
