#include <iostream>
#include <cstring>

#define __NEW_OVERLOAD__
#include "MemLeaDet.hpp"


typedef struct MemoryList{
  size_t _size; 
  bool _isArray;
  char* _file_name;
  unsigned int _line;
  struct MemoryList* _next, *_pre;
}MemoryList;

static unsigned long memory_allocated = 0;

static MemoryList root{
  0,
  false,
  nullptr,
  0,
  &root, &root,
};

unsigned int leak_detector::callcount = 0;

void* AllocateMemory(size_t size, bool array, char* file_name, unsigned int line){
  unsigned int newSize = sizeof(MemoryList) + size; //精妙所在
  MemoryList* newElem = (MemoryList*)malloc(newSize);
  if(NULL == newElem) return 0;

  newElem->_next = root._next;
  newElem->_pre = &root;
  newElem->_size = size;
  newElem->_isArray = array;
  newElem->_file_name = nullptr;

  if(file_name){
    newElem->_file_name = (char*)malloc(sizeof(file_name));
    if(NULL == newElem->_file_name) return 0;
    strcpy(newElem->_file_name, file_name);
  }
  newElem->_line = line;

  root._next->_pre = newElem;
  root._next = newElem;

  memory_allocated += size;
  return (char*)newElem + sizeof(MemoryList); //精妙所在
}

void DeleteMemory(void* ptr, bool array){
  MemoryList* curElem = (MemoryList*)((char*)ptr - sizeof(MemoryList));

  if(curElem->_isArray != array) return; //说明释放操作符类型不匹配

  //更新内存管理列表
  curElem->_pre->_next = curElem->_next;
  curElem->_next->_pre = curElem->_pre;
  memory_allocated -= curElem->_size;

  if(curElem->_file_name) free(curElem->_file_name); //释放存放文件信息时临时申请的内存
  free(curElem);
}

//重载 new / new[] 和 delete / delete[]
void* operator new(size_t size){
  return AllocateMemory(size, false, nullptr, 0);
}

void* operator new[](size_t size){
  return AllocateMemory(size, true, nullptr, 0);
}

void* operator new(size_t size, char* file_name, unsigned int line){
  return AllocateMemory(size, false, nullptr, 0);
}

void* operator new[](size_t size, char* file_name, unsigned int line){
  return AllocateMemory(size, true, nullptr, 0);
}

void operator delete(void* ptr) noexcept{
  DeleteMemory(ptr, false);
}

void operator delete[](void* ptr) noexcept{
  DeleteMemory(ptr, true);
}
