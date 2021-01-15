#include <cstring>
#include <cstdlib>
#include "obj_string.h"


//fnv-1a算法
uint32_t ObjString::hashString(char* str, uint32_t length) {
   uint32_t hashCode = 2166136261, idx = 0;
   while (idx < length) {
      hashCode ^= str[idx];
      hashCode *= 16777619;
      idx++;
   }
   return hashCode;
}

//为string计算哈希码并将值存储到string->hash
void ObjString::hashMyself() {
   this->hashCode = 
      hashString(this->value.str, this->value.length);
}

//以str字符串创建ObjString对象,允许空串""
ObjString::ObjString(std::shared_ptr<VM> vm, const char* str, size_t length) {
   //length为0时str必为NULL length不为0时str不为NULL
   ASSERT(length == 0 || str != nullptr, "str length do't match str!");
   //+1是为了结尾的'\0'
   this->value.length = length;
   this->value.str = ALLOCATE_ARRAY(vm, char, length+1);
   vm->incrementAllocatedBytes(sizeof(ObjString));
   vm->initAndAddObjHeader(&(this->objHeader), OT_STRING, vm->stringClass);

   //支持空字符串: str为null,length为0
   //如果非空则复制其内容
   if (length > 0) {
      memcpy(this->value.str, str, length);
   }
   this->value.str[length] = '\0';
   hashMyself();
}
