#pragma once

///////////////////////////////////////////////////////
// 科普：size_t
//  size_t 在32位平台下是unsigned int(4 byte), 在64位平台下是long unsigned int(8 byte) 
///////////////////////////////////////////////////////

void* operator new(size_t size, char* file_name, unsigned int line);
void* operator new[](size_t size, char* file_name, unsigned int line);

#ifndef __NEW_OVERLPAD__
#define new new(__FILE__, __LINE__)
#endif

class leak_detector{
public:
  static unsigned int callcount;
  leak_detector() noexcept{
    ++callcount;
  }
  ~leak_detector() noexcept{
    if(0 == --callcount){
      _leak_detector();
    } 
  }
private:
  static unsigned int _leak_detector() noexcept;
};

static leak_detector _exit_counter;

