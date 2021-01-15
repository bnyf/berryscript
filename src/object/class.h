#ifndef _OBJECT_CLASS_H
#define _OBJECT_CLASS_H

#include <string>
#include "obj_header.h"
#include "obj_string.h"
#include "../dataBuffer.h" 

class VM;
typedef struct Value_s Value_t;
typedef struct ObjClosure_s ObjClosure_t;

typedef enum {
   MT_NONE,     // 空方法类型,并不等同于undefined
   MT_PRIMITIVE,    // 在vm中用c实现的原生方法
   MT_SCRIPT,	// 脚本中定义的方法
   MT_FN_CALL,  // 有关函数对象的调用方法,用来实现函数重载
} MethodType;   // 方法类型

#define INIT_VALUE_OF_TPYE(vt) (Value_t(vt, 0))

#define BOOL_INIT_VALUE(boolean) (boolean ? INIT_VALUE_OF_TPYE(VT_TRUE) : INIT_VALUE_OF_TPYE(VT_FALSE))
#define VALUE_TO_BOOL(value) ((value).type == VT_TRUE ? true : false)

#define NUM_TO_VALUE(num) ((Value_t){VT_NUM, num})
#define VALUE_TO_NUM(value) ((value).num)

#define OBJ_TO_VALUE(objPtr) ({ \
   Value_t value; \
   value.type = VT_OBJ; \
   value.objHeader = (ObjHeader*)(objPtr); \
   value; \
})

#define VALUE_TO_OBJ(value) ((value).objHeader)
#define VALUE_TO_OBJSTR(value) ((ObjString*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJFN(value) ((ObjFn_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJRANGE(value) ((ObjRange_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJINSTANCE(value) ((ObjInstance_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJLIST(value) ((ObjList_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMAP(value) ((ObjMap_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJCLOSURE(value) ((ObjClosure_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJTHREAD(value) ((ObjThread_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMODULE(value) ((ObjModule_t*)VALUE_TO_OBJ(value))
#define VALUE_TO_CLASS(value) ((Class_t*)VALUE_TO_OBJ(value))

#define VALUE_IS_UNDEFINED(value) ((value).type == VT_UNDEFINED)
#define VALUE_IS_NULL(value) ((value).type == VT_NULL)
#define VALUE_IS_TRUE(value) ((value).type == VT_TRUE)
#define VALUE_IS_FALSE(value) ((value).type == VT_FALSE)
#define VALUE_IS_NUM(value) ((value).type == VT_NUM)
#define VALUE_IS_OBJ(value) ((value).type == VT_OBJ)
#define VALUE_IS_CERTAIN_OBJ(value, objType) (VALUE_IS_OBJ(value) && VALUE_TO_OBJ(value)->type == objType)
#define VALUE_IS_OBJSTR(value) (VALUE_IS_CERTAIN_OBJ(value, OT_STRING))
#define VALUE_IS_OBJINSTANCE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_INSTANCE))
#define VALUE_IS_OBJCLOSURE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_CLOSURE))
#define VALUE_IS_OBJRANGE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_RANGE))
#define VALUE_IS_CLASS(value) (VALUE_IS_CERTAIN_OBJ(value, OT_CLASS))
#define VALUE_IS_0(value) (VALUE_IS_NUM(value) && (value).num == 0)

//原生方法指针
typedef bool (*Primitive)(VM* vm, Value_t* args);

typedef struct Method_s{
   MethodType type;  // union中的值由type的值决定
   union {      
      // 指向脚本方法所关联的c实现
      Primitive primFn;

      // 指向脚本代码编译后的ObjClosure或ObjFn
      ObjClosure_t* obj;
   };
} Method_t;

//类是对象的模板
typedef struct Class_s{
   ObjHeader_t objHeader;
   struct Class_s* superClass; //父类
   uint32_t fieldNum;	   //本类的字段数,包括基类的字段数
   DataBuffer<Method_t> methods;   //本类的方法
   std::string* name;   //类名
} Class_t;  //对象类

typedef struct Value_s{
   public:
      ValueType type;
      union {
         double num;
         ObjHeader_t* objHeader;
      };
      Value_s(ValueType _type = VT_UNDEFINED, double _num = 0) : type(_type), num(_num){}
      bool operator = (const Value_s &x) {
         //类型不同则无须进行后面的比较
         if (this->type != x.type) {
            return false;
         }
         
         //同为数字,比较数值
         if (this->type == VT_NUM) {
            return this->num == x.num;
         }
         
         //同为对象,若所指的对象是同一个则返回true
         if (this->objHeader == x.objHeader) {
            return true;
         }
         
         //对象类型不同无须比较
         if (this->objHeader->type != x.objHeader->type) {
            return false;
         }

         //以下处理类型相同的对象
         //若对象同为字符串
         if (this->objHeader->type == OT_STRING) {
            ObjString* strA = VALUE_TO_OBJSTR(*this);
            ObjString* strB = VALUE_TO_OBJSTR(x);
            return (strA->value.length == strB->value.length &&
            memcmp(strA->value.str, strB->value.str, strA->value.length) == 0);
         }

         //若对象同为range
         // if (this->objHeader->type == OT_RANGE) {
         //    ObjRange* rgA = VALUE_TO_OBJRANGE(*this);
         //    ObjRange* rgB = VALUE_TO_OBJRANGE(x);
         //    return (rgA->from == rgB->from && rgA->to == rgB->to);
         // }

         return false;  //其它对象不可比较
      }
} Value_t; //通用的值结构

typedef union {
   uint64_t bits64;
   uint32_t bits32[2];
   double num;
} Bits64;

#define CAPACITY_GROW_FACTOR 4 
#define MIN_CAPACITY 64

#endif