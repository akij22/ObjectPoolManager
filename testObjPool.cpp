#include "ObjectPoolManager.hh"
#include <iostream>
#include <memory>
#include <string>

// FOR TESTING
struct Person {
  std::string name;
  int age;

  // Constructor

  Person() : name("Default"), age(100) {}
  Person(std::string init_name, int init_age)
      : name(init_name), age(init_age) {}

  ~Person() { std::cout << "Deleting Person obj" << std::endl; }

  // friend std::ostream std::operator<<(std::ostream &out) {}
};

int main() {

  auto mp1 = std::make_shared<ObjectPoolManager<Person>>(10);

  // Define a scope for `h` to test the `.release()` method
  {

    std::cout << mp1->size() << std::endl;

    // Test `.constuct` method
    auto h = mp1->construct("Checking...",
                            22); // requiring a block and save it into `h`
    auto h2 = mp1->construct("2", 2);

    auto h3 = mp1->construct("2", 2);

    auto h4 = mp1->construct("2", 2);

    auto h5 = mp1->construct("2", 2);

    auto h6 = mp1->construct("2", 2);

    auto h7 = mp1->construct("2", 2);

    auto h8 = mp1->construct("2", 2);

    auto h9 = mp1->construct("2", 2);

    auto h10 = mp1->construct("2", 2);

    std::cout << mp1->exhausted() << std::endl;

    std::cout << mp1->size() << std::endl;

    // TODO write `expand` method for expanding the size of free_list
    auto h11 = mp1->construct("2", 2);

    std::cout << mp1->size() << std::endl;

  } // the deleter must be called

  // Checking if pool_pointers constraint is called
  // Person *checkThrow = new Person("check1", 100);
  // mp1->release(checkThrow);

  return 0;
} // MemoryPoolManager is out-of-scope, it must be deleted
