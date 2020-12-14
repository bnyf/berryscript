#include "dataBuffer.h"

template<class T>
DataBuffer<T>::DataBuffer() {
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
        this->datas = vm->memManager(this->datas, oldSize, newSize);
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
    vm->memManager(this->datas, oldSize, 0);
    this->datas = nullptr;
    this->count = capacity = 0;
}

template<class T>
T* DataBuffer<T>::allocate() {
    return (T *)vm->memManager(NULL, 0, sizeof(T));
}

template<class T>
T* DataBuffer<T>::allocateArray(uint32_t count) {
    return (T *)vm->memManager(NULL, 0, sizeof(T) * count);
}

template<class T>
T* DataBuffer<T>::deAllocateArray(T* dataPtr, uint32_t count) {
    return (T *)vm->memManager(dataPtr, sizeof(T) * count, 0);
}

template<class T>
T* DataBuffer<T>::deAllocate(T* dataPtr) {
    return (T *)vm->memManager(dataPtr, 0, 0);
}