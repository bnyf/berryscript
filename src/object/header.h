#ifndef _HEADER_H
#define _HEADER_H

class VM;
class BerryClass;
class ObjClosure;

struct Value;

#include "stdint.h"

typedef enum {
   OT_CLASS,   //此项是class类型,以下都是object类型
   OT_LIST,
   OT_MAP,
   OT_MODULE,
   OT_RANGE,
   OT_STRING,
   OT_UPVALUE,
   OT_FUNCTION,
   OT_CLOSURE,
   OT_INSTANCE,
   OT_THREAD
} ObjType;  //对象类型

class ObjHeader{
   public:
      ObjType type;
      bool isDark;	   //对象是否可达
      BerryClass* berryClass;   //对象所属的类
      ObjHeader* next;   //用于链接所有已分配对象
};	  //对象头,用于记录元信息和垃圾回收

typedef enum {
   MT_NONE,     // 空方法类型,并不等同于undefined
   MT_PRIMITIVE,    // 在vm中用c实现的原生方法
   MT_SCRIPT,	// 脚本中定义的方法
   MT_FN_CALL,  // 有关函数对象的调用方法,用来实现函数重载
} MethodType;   // 方法类型

//原生方法指针
typedef bool (*Primitive)(VM* vm, Value* args);

struct Method{
   MethodType type;  // union中的值由type的值决定
   union {      
      // 指向脚本方法所关联的c实现
      Primitive primFn;

      // 指向脚本代码编译后的ObjClosure或ObjFn
      ObjClosure* obj;
   };
};

typedef enum {
    VT_UNDEFINED,
    VT_NULL,
    VT_FALSE,
    VT_TRUE,
    VT_NUM,
    VT_OBJ   //值为对象,指向对象头
} ValueType;     //value类型

struct Value{
    ValueType type;
    union {
        double num;
        ObjHeader* objHeader;
    };
    Value(ValueType _type = VT_UNDEFINED, double _num = 0);
    bool operator == (const Value &x);
}; //通用的值结构

typedef union {
   uint64_t bits64;
   uint32_t bits32[2];
   double num;
} Bits64;

#define INIT_VALUE_OF_TPYE(vt) (Value(vt, 0))

#define BOOL_INIT_VALUE(boolean) (boolean ? INIT_VALUE_OF_TPYE(VT_TRUE) : INIT_VALUE_OF_TPYE(VT_FALSE))
#define VALUE_TO_BOOL(value) ((value).type == VT_TRUE ? true : false)

#define NUM_TO_VALUE(num) (Value(VT_NUM, num))
#define VALUE_TO_NUM(value) ((value).num)

#define OBJ_TO_VALUE(objPtr) ({ \
   Value value; \
   value.type = VT_OBJ; \
   value.objHeader = (ObjHeader*)(objPtr); \
   value; \
})

#define VALUE_TO_OBJ(value) ((value).objHeader)
#define VALUE_TO_OBJSTR(value) ((ObjString*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJFN(value) ((ObjFn*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJRANGE(value) ((ObjRange*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJINSTANCE(value) ((ObjInstance*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJLIST(value) ((ObjList*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMAP(value) ((ObjMap*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJCLOSURE(value) ((ObjClosure*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJTHREAD(value) ((ObjThread*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMODULE(value) ((ObjModule*)VALUE_TO_OBJ(value))
#define VALUE_TO_CLASS(value) ((Class*)VALUE_TO_OBJ(value))

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

#define CAPACITY_GROW_FACTOR 4 
#define MIN_CAPACITY 64

#endif
