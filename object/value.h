#ifndef _BERRYSCRIPT_VALUE_H
#define _BERRYSCRIPT_VALUE_H
#include "utils.h"

typedef enum {
	VT_UNDEFINED,
	VT_NULL,
	VT_FALSE,
	VT_TRUE,
	VT_NUM,
	VT_MAP, // object
	VT_STRING,
	VT_LIST,
	VT_UPVALUE,
	VT_FUNCTION,
	VT_CLOSURE,
} ValueType;	  //value类型

typedef struct {
	ValueType type;
	bool isDark;	   //对象是否可达
	union {
		void* valuePtr;
		double num;
	};
	Value* next;
} Value;	//通用的值结构

DECLARE_BUFFER_TYPE(Value)

Value* initValue(VM* vm, ValueType valueType, void *v);
bool valueIsEqual(Value a, Value b);

#endif
