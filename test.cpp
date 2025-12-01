#include "MemoryPoolManager.hh"
#include <iostream>
#include <memory>

int main() {

  auto mp1 = std::make_shared<MemoryPoolManager<int>>(1024, 10);

  // Define a scope for `h` to test the `.release()` method
  {
    auto h = mp1->acquire(); // requiring a block and save it into `h`
  } // the deleter must be called
  return 0;
} // MemoryPoolManager is out-of-scope, it must be deleted
