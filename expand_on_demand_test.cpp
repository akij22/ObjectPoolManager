#include "ObjectPoolManager.hh"
#include <iostream>

struct TrackingPerson {
  static int constructions;
  static int destructions;

  std::string name;
  int age;

  TrackingPerson(std::string init_name = "Default", int init_age = 0)
      : name(std::move(init_name)), age(init_age) {
    ++constructions;
  }

  ~TrackingPerson() { ++destructions; }
};

int main() {
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(1);

  {
    auto first = pool->construct("One", 1);

    std::cout << "Calling construct, automatic expand must be called"
              << std::endl;
    auto second = pool->construct("Two", 2);
  }
}
