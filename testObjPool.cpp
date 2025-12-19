#include "ObjectPoolManager.hh"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/*
Improvements to consider for ObjectPoolManager:
1. Provide a definition for the default constructor or remove its declaration.
2. Initialize every member of Stats (usedBlocks, freeBlocks, allocationCount,
   deallocationCount) at construction to avoid undefined reads.
3. Avoid throwing from PoolCustomDeleter when ptr is nullptr to prevent
   terminate during stack unwinding; just return early instead.
4. Ensure the destructor reclaims raw memory for blocks that are still checked
   out when the pool is destroyed to prevent leaks.
5. Keep release() private (or guard double releases) so callers cannot push the
   same pointer multiple times or bypass the handle semantics.
6. Keep stats/allocationCount consistent inside expand() and consider making
   the growth factor configurable for large T types.
*/

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

int TrackingPerson::constructions = 0;
int TrackingPerson::destructions = 0;

void reset_tracking() {
  TrackingPerson::constructions = 0;
  TrackingPerson::destructions = 0;
}

void test_basic_acquire_and_release() {
  reset_tracking();
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(2);

  {
    auto first = pool->construct("Alice", 30);
    auto second = pool->construct("Bob", 40);

    assert(first->name == "Alice");
    assert(second->age == 40);
    assert(pool->exhausted());
    assert(pool->size() == 0);
  }

  assert(pool->size() == 2);
  assert(!pool->exhausted());
  assert(TrackingPerson::constructions == TrackingPerson::destructions);
}

void test_expand_on_demand() {
  reset_tracking();
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(1);

  {
    auto first = pool->construct("One", 1);
    assert(pool->size() == 0);

    auto second = pool->construct("Two", 2);
    assert(second->name == "Two");
    assert(pool->size() == 1);
  }

  assert(pool->size() == 3);
}

void test_handles_stored_in_containers() {
  reset_tracking();
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(3);

  std::vector<ObjectPoolManager<TrackingPerson>::Handle> handles;
  handles.reserve(3);

  for (int i = 0; i < 3; ++i) {
    handles.emplace_back(pool->construct("User" + std::to_string(i), i));
  }

  assert(pool->exhausted());

  handles.pop_back();
  assert(pool->size() == 1);

  handles.clear();
  assert(pool->size() == 3);
}

void test_rejects_foreign_pointer() {
  reset_tracking();
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(1);

  auto fake = new TrackingPerson("Intruder", 99);
  bool threw = false;

  try {
    pool->release(fake);
  } catch (const std::invalid_argument &) {
    threw = true;
  }

  delete fake;
  assert(threw);
}

int main() {
  test_basic_acquire_and_release();
  test_expand_on_demand();
  test_handles_stored_in_containers();
  test_rejects_foreign_pointer();

  std::cout << "All object pool tests passed" << std::endl;
  return 0;
}
