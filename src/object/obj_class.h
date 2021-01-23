#ifndef _CLASS_H
#define _CLASS_H

#include <string>
#include "header.h"
#include "../dataBuffer.h" 

struct BerryString;

class VM;
struct Value;

class ObjInstance : public ObjHeader{
    ObjHeader objHeader;
    // 具体的字段
    int fieldNum;
    Value *fields;
    ObjInstance(VM* vm, BerryClass* berryClass);
};	//对象实例

// 类是对象的模板
class BerryClass : public ObjHeader{
    public:
        BerryClass* superClass; //父类
        uint32_t fieldNum;	   //本类的字段数,包括基类的字段数
        DataBuffer<Method> methods;   //本类的方法
        std::string name;   //类名
};  //对象类

class ObjModule : public ObjHeader{
    DataBuffer<BerryString> moduleVarName;   // 模块中的模块变量名
    DataBuffer<Value> moduleVarValue;  // 模块中的模块变量值
    std::string name;   // 模块名
    ObjModule(std::shared_ptr<VM>vm, const char* modName);
}; //模块对象

#endif