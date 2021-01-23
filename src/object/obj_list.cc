#include "obj_list.h"

// 新建list对象, 元素个数为elementNum
ObjList::ObjList(std::shared_ptr<VM> vm, uint32_t elementNum)
 : elements(vm) {
   vm->incrementAllocatedBytes(sizeof(ObjList));
   // 先分配内存,后调用initObjHeader,避免gc无谓的遍历
   if (elementNum > 0) {
      this->elements.datas = ALLOCATE_ARRAY(vm, Value, elementNum);
   }
   this->elements.capacity = this->elements.count = elementNum;
   vm->initAndAddObjHeader(this, OT_LIST, vm->listClass);
}

//在objlist中索引为index处插入value, 类似于list[index] = value
void ObjList::insertElement(uint32_t index, Value value) {
   if (index > this->elements.count - 1) {
      RUN_ERROR("index out bounded!");
   }

   // 准备一个Value的空间以容纳新元素产生的空间波动
   // 即最后一个元素要后移1个空间
   this->elements.bufferAdd(INIT_VALUE_OF_TPYE(VT_NULL));
   
   // 下面使index后面的元素整体后移一位
   for(int i = this->elements.count;i > index;++i) {
      this->elements.datas[i] = this->elements.datas[i - 1];
   }

   //在index处插入数值
   this->elements.datas[index] = value;
}

//调整list容量
void ObjList::shrinkList(uint32_t newCapacity) {
   uint32_t oldSize = this->elements.capacity * sizeof(Value);
   uint32_t newSize = newCapacity * sizeof(Value);
   vm->memManager(this->elements.datas, oldSize, newSize);
   this->elements.capacity = newCapacity;
}

//删除list中索引为index处的元素,即删除list[index]
Value ObjList::removeElement(uint32_t index) {
   Value valueRemoved = this->elements.datas[index];

   //使index后面的元素前移一位,覆盖index处的元素
   uint32_t idx = index;
   for(int i=index;i<this->elements.count;++i) {
      this->elements.datas[i] = this->elements.datas[i + 1];
      idx++;
   }

   //若容量利用率过低就减小容量
   uint32_t _capacity = this->elements.capacity / CAPACITY_GROW_FACTOR;
   if (_capacity > this->elements.count) {
      shrinkList(_capacity);
   }

   this->elements.count--;
   return valueRemoved;
}
