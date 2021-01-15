#ifndef _DATABUFFER_H
#define _DATABUFFER_H

#include <cstdint>
#include <memory>
#include "utils.h"

class VM;

template<class T>
class DataBuffer {
      public:
            DataBuffer(std::shared_ptr<VM> _vm);
            void bufferMultiAdd(T data, uint32_t fillCount);
            void bufferAdd(T data);
            void bufferClear();
            // T* allocate(); //新分配一个T元素
            // T* allocateArray(uint32_t count); //新分配一个长度为count的T元素数组
            // T* deAllocateArray(T* dataPtr, uint32_t count); //删除dataPtr开始的count长度个T元素
            // T* deAllocate(T* dataPtr); //释放地址dataPtr

      private:
            /* 数据缓冲区 */ 
            T* datas;
            /*缓冲区中已使用的元素个数*/
            uint32_t count;
            /*缓冲区的容量*/
            uint32_t capacity;
            std::shared_ptr<VM> vm;
};

template<class T>
DataBuffer<T>::DataBuffer(std::shared_ptr<VM> _vm) : vm(_vm) {
    this->datas = nullptr;
    this->count = capacity = 0;
}

template<class T>
void DataBuffer<T>::bufferMultiAdd(T data, uint32_t fillCount) {
    uint32_t newCounts = this->count + fillCount;
    if(newCounts > this->capacity) {
        uint32_t oldSize = this->capacity * sizeof(T);
        this->capacity = ceilToPowerOf2(newCounts);
        uint32_t newSize = this->capacity * sizeof(T);
        this->datas = (T *)this->vm->memManager(this->datas, oldSize, newSize);
    }
    for(int i=this->count;i<newCounts;++i) {
        this->datas[i] = data;
    }
    this->count = newCounts;
}

template<class T>
void DataBuffer<T>::bufferAdd(T data) {
    bufferMultiAdd(data, 1);
}

template<class T>
void DataBuffer<T>::bufferClear() {
    uint32_t oldSize = this->capacity * sizeof(T);
    this->vm->memManager(this->datas, oldSize, 0);
    this->datas = nullptr;
    this->count = capacity = 0;
}

// template<class T>
// T* DataBuffer<T>::allocate() {
//     return (T *)vm->memManager(NULL, 0, sizeof(T));
// }

// template<class T>
// T* DataBuffer<T>::allocateArray(uint32_t count) {
//     return (T *)vm->memManager(NULL, 0, sizeof(T) * count);
// }

// template<class T>
// T* DataBuffer<T>::deAllocateArray(T* dataPtr, uint32_t count) {
//     return (T *)vm->memManager(dataPtr, sizeof(T) * count, 0);
// }

// template<class T>
// T* DataBuffer<T>::deAllocate(T* dataPtr) {
//     return (T *)vm->memManager(dataPtr, 0, 0);
// }

#endif