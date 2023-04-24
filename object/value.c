#include "value.h"
#include "vm.h"

//初始化对象头
Value* initValue(VM* vm, ValueType valueType, void *v) {
	Value *value = ALLOCATE(vm, Value);
	value->type = valueType;
	value->isDark = false;
	value->valuePtr = v;
	value->next = vm->allValues;
	vm->allValues = value;
}

//判断a和b是否相等
bool valueIsEqual(Value &a, Value &b) {
	//类型不同则无须进行后面的比较
	if (a->type != b->type) {
		return false;
	}
	
	//同为数字,比较数值
	if (a.type == VT_NUM) {
		return a.num == b.num;
	}

	//以下处理类型相同的对象
	//若对象同为字符串
	if (a->type == VT_STRING) {
		ObjString* strA = VALUE_TO_OBJSTR(a);
		ObjString* strB = VALUE_TO_OBJSTR(b);
		return (strA->value.length == strB->value.length &&
		 memcmp(strA->value.start, strB->value.start, strA->value.length) == 0);
	}

	return false;  //其它对象不可比较
}