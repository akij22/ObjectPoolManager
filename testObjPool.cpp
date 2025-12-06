#include "ObjectPoolManager.hh"
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

  Person() {
    name = "";
    age = 0;
  }

  ~Person() { std::cout << "Deleting Person obj" << std::endl; }
};

int main() {

  auto mp1 = std::make_shared<ObjectPoolManager<Person>>(10);

  // Define a scope for `h` to test the `.release()` method
  {

    // `h` is now a unique_ptr to int object
    auto h = mp1->construct("Checking...",
                            22); // requiring a block and save it into `h`

    // TODO Implement constuct() method for avoiding this
    //
    // In this case we are allocating into `h.get()` memory the ptr1 pointer

    // Person *person = mp1->construct("Name", age);

    std::cout << "Value of the object: " << h->name << std::endl;

    std::cout << "Address of wrapped ptr: " << h.get() << std::endl;

    std::cout << "Size of the object pool: " << mp1->size() << std::endl;

  } // the deleter must be called

  std::cout << "Size of the object pool after release: " << mp1->size()
            << std::endl;

  return 0;
} // MemoryPoolManager is out-of-scope, it must be deleted
