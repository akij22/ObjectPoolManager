#include "MemoryPoolManager.hh"
#include <iostream>
#include <memory>

int main() {

  auto mp1 = std::make_shared<MemoryPoolManager<int>>(1024, 10);

  {
    auto h = mp1->acquire(); // requiring a block and save it into `h`
  } // the deleter must be called
  return 0;
} // MemoryPoolManager is out-of-scope, it must be deleted
