#include "class.h"
#include "common.h"
#include "string.h"
#include "core.h"
#include "vm.h"

DEFINE_BUFFER_METHOD(Method)

//新建一个裸类
Class* newRawClass(VM* vm, const char* name, uint32_t fieldNum) {
	Class* class = ALLOCATE(vm, Class); 

	//裸类没有元类
	initObjHeader(vm, &class->objHeader, OT_CLASS, NULL);
	class->name = newObjString(vm, name, strlen(name));
	class->fieldNum = fieldNum;
	class->superClass = NULL;	//默认没有基类

	pushTmpRoot(vm, (ObjHeader*)class);
	MethodBufferInit(&class->methods);
	popTmpRoot(vm);

	return class;
}

//创建一个类
Class* newClass(VM* vm, ObjString* className, uint32_t fieldNum, Class* superClass) {
	//10表示strlen(" metaClass"
	#define MAX_METACLASS_LEN MAX_ID_LEN + 10
	char newClassName[MAX_METACLASS_LEN] = {'\0'};
	#undef MAX_METACLASS_LEN

	memcpy(newClassName, className->value.start, className->value.length);
	memcpy(newClassName + className->value.length, " metaclass", 10);

	//先创建子类的meta类
	Class* metaclass = newRawClass(vm, newClassName, 0);
	metaclass->objHeader.class = vm->classOfClass;

	pushTmpRoot(vm, (ObjHeader*)metaclass);
	//绑定classOfClass为meta类的基类
	//所有类的meta类的基类都是classOfClass
	bindSuperClass(vm, metaclass, vm->classOfClass);
	
	//最后再创建类
	memcpy(newClassName, className->value.start, className->value.length);
	newClassName[className->value.length] = '\0';
	Class* class = newRawClass(vm, newClassName, fieldNum);
	pushTmpRoot(vm, (ObjHeader*)class);

	class->objHeader.class = metaclass;
	bindSuperClass(vm, class, superClass);

	popTmpRoot(vm);	// metaclass
	popTmpRoot(vm);	// class
	
	return class;
}

//数字等Value也被视为对象,因此参数为Value.获得对象obj所属的类
inline Class* getClassOfObj(VM* vm, Value object) {
	switch (object.type) {
		case VT_NULL:
	 return vm->nullClass;
		case VT_FALSE:
		case VT_TRUE:
	 return vm->boolClass;
		case VT_NUM:
	 return vm->numClass;
		case VT_OBJ:
	 return VALUE_TO_OBJ(object)->class;
		default:
	NOT_REACHED(); 
	}
	return NULL;
}
