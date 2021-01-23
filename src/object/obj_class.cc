#include "obj_class.h"
#include "obj_string.h"
#include "obj_range.h"

ObjModule::ObjModule(std::shared_ptr<VM>vm, const char* modName) : moduleVarName(vm), moduleVarValue(vm) {
    //ObjModule是元信息对象,不属于任何一个类
    vm->initAndAddObjHeader(this, OT_MODULE, NULL);
    if (modName != NULL) {
        this->name = std::string(modName, strlen(modName));
    }
}

ObjInstance :: ObjInstance(VM* vm, BerryClass* berryClass) {
    this->fieldNum = berryClass->fieldNum;
    //参数class主要作用是提供类中field的数目
    this->fields = ALLOCATE_ARRAY(vm, Value, this->fieldNum);
    //初始化field为NULL
    for(int i=0;i<fieldNum;++i) {
        (this->fields)[i] = INIT_VALUE_OF_TPYE(VT_NULL);
    }

    //在此关联对象的类为参数class
    vm->initAndAddObjHeader(&this->objHeader, OT_INSTANCE, berryClass);
}
