#ifndef _OBJECT_STRING_H
#define _OBJECT_STRING_H

#include "value.h"

typedef struct {
	uint32_t hashCode;  //字符串的哈希值
	CharValue value;
} ObjString;

uint32_t hashString(char* str, uint32_t length);
void hashObjString(ObjString* objString);
Value* newObjString(VM* vm, const char* str, uint32_t length);
#endif
