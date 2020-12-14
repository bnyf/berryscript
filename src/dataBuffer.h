#ifndef _DATABUFFER_H
#define _DATABUFFER_H

#include <cstdint>
#include "vm.h"
#include "utils.h"

template<class T>
class DataBuffer {
public:
      DataBuffer();
      void bufferMultiAdd(T data, uint32_t fillCount);
      void bufferAdd(T data);
      void bufferClear();
      T* allocate(); //新分配一个T
      T* allocateArray(uint32_t count); //新分配一个长度为count的T数组
      T* deAllocateArray(T* dataPtr, uint32_t count); //删除dataPtr开始的count长度个T 
      T* deAllocate(T* dataPtr); //释放地址dataPtr

private:
      /* 数据缓冲区 */ 
      T* datas;
      /*缓冲区中已使用的元素个数*/
      uint32_t count;
      /*缓冲区的容量*/
      uint32_t capacity;
      VM *vm;
};

#endif