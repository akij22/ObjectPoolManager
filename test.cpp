#include "MemoryPoolManager.hh"
#include <iostream>
#include <memory>
#include <string>

// FOR TESTING
struct Person {
  std::string name;
  int age;

  // Constructor
  Person(std::string init_name, int init_age)
      : name(init_name), age(init_age) {}

  ~Person() { std::cout << "Deleting Person obj" << std::endl; }
};

int main() {

  auto mp1 = std::make_shared<MemoryPoolManager<int>>(1024, 10);

  // Define a scope for `h` to test the `.release()` method
  {
    auto h = mp1->acquire(); // requiring a block and save it into `h`

    int *ptr1 = new (h.get()) int(20);

    std::cout << *ptr1 << std::endl;

    std::cout << "Address of wrapped ptr: " << h.get() << std::endl;

    std::cout << "Address unique ptr " << h << std::endl;

    std::cout << "Size: " << mp1->size() << std::endl;

  } // the deleter must be called

  return 0;
} // MemoryPoolManager is out-of-scope, it must be deleted
