#include "MemoryPoolManager.hh"
#include <iostream>

int main() {

  MemoryPoolManager<int> mp1(
      1024, 10); // Allocating 10 blocks of 1024 size for each one

  {
    auto h = mp1.acquire(); // requiring a block and save it into `h`
  } // the deleter must be called
  return 0;
} // MemoryPoolManager is out-of-scope, it must be deleted
