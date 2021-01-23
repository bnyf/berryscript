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
    
        /* 数据缓冲区 */ 
        T* datas;
        /*缓冲区中已使用的元素个数*/
        uint32_t count;
        /*缓冲区的容量*/
        uint32_t capacity;

    private:
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

#endif