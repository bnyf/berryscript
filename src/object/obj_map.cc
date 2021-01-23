#include "obj_map.h"
#include "../utils.h"
#include "header.h"
#include "obj_class.h"
#include "../vm.h"
#include "obj_range.h"
#include "obj_string.h"

ObjMap::ObjMap(std::shared_ptr<VM> vm) {
    this->vm = vm;
    vm->incrementAllocatedBytes(sizeof(ObjMap));
    vm->initAndAddObjHeader(this, OT_MAP, vm->mapClass);
    this->capacity = this->count = 0;
    this->entries = NULL;
}

//计算数字的哈希码
static uint32_t hashNum(double num) {
   Bits64 bits64;
   bits64.num = num;
   return bits64.bits32[0] ^ bits64.bits32[1];
}

//计算对象的哈希码
static uint32_t hashObj(ObjHeader* objHeader) {
    switch (objHeader->type) {
        case OT_CLASS:  //计算class的哈希值
            return hashString((static_cast<BerryClass *>(objHeader))->name.c_str(),
                (static_cast<BerryClass *>(objHeader))->name.size());
        case OT_RANGE: { //计算range对象哈希码
	        ObjRange* objRange = static_cast<ObjRange *>(objHeader);
	        return hashNum(objRange->from) ^ hashNum(objRange->to);
        }
        case OT_STRING:  //对于字符串,直接返回其hashCode
            return (static_cast<ObjString *>(objHeader))->hashCode;
        default:
            RUN_ERROR("the hashable should be objstring, objrange or berryclass.");
   }
   return 0;
}

//根据value的类型调用相应的哈希函数
static uint32_t hashValue(Value value) {
    switch (value.type) {
        case VT_FALSE:
            return 0;
        case VT_NULL:
            return 1;
        case VT_TRUE:
            return 2;
        case VT_NUM:
            return hashNum(value.num);
        case VT_OBJ:
            return hashObj(value.objHeader);
        default:
            RUN_ERROR("unsupport type hashed!");
   }
   return 0;
}

//在entries中添加entry,如果是新的key则返回true
static bool addEntry(Entry* entries, uint32_t capacity, Value key, Value value) {
    uint32_t index = hashValue(key) % capacity;

    //通过开放探测法去找可用的slot
    while (true) {
        //找到空闲的slot,说明目前没有此key,直接赋值返回
        if (entries[index].key.type == VT_UNDEFINED) {
            entries[index].key = key;
            entries[index].value = value;
            return true;	   //新的key就返回true
        } else if (entries[index].key == key) { //key已经存在,仅仅更新值就行
            entries[index].value = value;
            return false;	// 未增加新的key就返回false
        }

        //开放探测定址,尝试下一个slot
        index = (index + 1) % capacity;
    }
}

bool ObjMap::mapSet(Value key, Value value) {
    return addEntry(this->entries, this->capacity, value, key);
}

//在objMap中查找key对应的entry
Entry* ObjMap::findEntry(Value key) {
    //objMap为空则返回null
    if (capacity == 0) {
        return NULL;
    }

    //以下开放定址探测
    //用哈希值对容量取模计算槽位(slot)
    uint32_t index = hashValue(key) % capacity;
    Entry* entry; 
    while (true) {
        entry = entries + index;

        //若该slot中的entry正好是该key的entry,找到返回
        if (entry->key == key) {
            return entry;
        }

        //key为VT_UNDEFINED且value为VT_TRUE表示探测链未断,可继续探测.
        //key为VT_UNDEFINED且value为VT_FALSE表示探测链结束,探测结束.
        if (VALUE_IS_UNDEFINED(entry->key) && VALUE_IS_FALSE(entry->value)) {
            return NULL;    //未找到
        }

        //继续向下探测
        index = (index + 1) % this->capacity;
    }
}

Value ObjMap::mapGet(Value key) {
    Entry* entry = findEntry(key);
    if (entry == NULL) {
        return INIT_VALUE_OF_TPYE(VT_UNDEFINED);
    }
    return entry->value;
}

void ObjMap::clearMap() {
    DEALLOCATE_ARRAY(this->vm, this->entries, this->count);
    this->entries = NULL;
    this->capacity = this->count = 0;
}

// 使对象objMap的容量调整到newCapacity
void ObjMap::resizeMap(uint32_t newCapacity) {
    // 先建立个新的entry数组
    Entry* newEntries = ALLOCATE_ARRAY(vm, Entry, newCapacity);
    for (int i=0;i<newCapacity;++i) {
        newEntries[i].key = INIT_VALUE_OF_TPYE(VT_UNDEFINED);
        newEntries[i].value = INIT_VALUE_OF_TPYE(VT_FALSE);
    }
    
    // 再遍历老的数组,把有值的部分插入到新数组 
    if (this->capacity > 0) {
        Entry* entryArr = this->entries;
        for (int i=0;i<this->capacity;++i) {
            //该slot有值
            if (entryArr[i].key.type != VT_UNDEFINED) {
                addEntry(newEntries, newCapacity,
                entryArr[i].key, entryArr[i].value);
            }
        }
    }

   // 将老entry数组空间回收
   DEALLOCATE_ARRAY(vm, this->entries, this->count);
   this->entries = newEntries;     //更新指针为新的entry数组
   this->capacity = newCapacity;    //更新容量 
}

Value ObjMap::removeKey(Value key) {
    Entry* entry = findEntry(key);

    if (entry == NULL) {
        return INIT_VALUE_OF_TPYE(VT_NULL);
    }

    //设置开放定址的伪删除
    Value value = entry->value;
    entry->key = INIT_VALUE_OF_TPYE(VT_UNDEFINED); 
    entry->value = INIT_VALUE_OF_TPYE(VT_TRUE);   //值为真,伪删除

    this->count--;  
    if (this->count == 0) { //若删除该entry后map为空就回收该空间
        clearMap();
    } else if (this->count < this->capacity / (CAPACITY_GROW_FACTOR) * MAP_LOAD_PERCENT &&
        this->count > MIN_CAPACITY) {   //若map容量利用率太低,就缩小map空间
        uint32_t newCapacity = this->capacity / CAPACITY_GROW_FACTOR;
        if (newCapacity < MIN_CAPACITY) {
            newCapacity = MIN_CAPACITY;
        }
        resizeMap(newCapacity);
    }

    return value;
}