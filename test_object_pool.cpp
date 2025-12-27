#include "ObjectPoolManager.hh"
#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <thread>
#include <vector>

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

class ObjectPoolTest : public ::testing::Test {
protected:
  void SetUp() override {
    TrackingPerson::constructions = 0;
    TrackingPerson::destructions = 0;
  }
};

TEST_F(ObjectPoolTest, BasicAcquireAndRelease) {
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(2);

  {
    auto first = pool->construct("Alice", 30);
    auto second = pool->construct("Bob", 40);

    EXPECT_EQ(first->name, "Alice");
    EXPECT_EQ(second->age, 40);
    EXPECT_TRUE(pool->exhausted());
    EXPECT_EQ(pool->size(), 0);
  }

  EXPECT_EQ(pool->size(), 2);
  EXPECT_FALSE(pool->exhausted());
  EXPECT_EQ(TrackingPerson::constructions, TrackingPerson::destructions);
}

TEST_F(ObjectPoolTest, ExpandOnDemand) {
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(1);

  {
    auto first = pool->construct("One", 1);
    EXPECT_EQ(pool->size(), 0);

    std::cout << "Calling construct, automatic expand must be called"
              << std::endl;
    auto second = pool->construct("Two", 2);
    EXPECT_EQ(second->name, "Two");
    EXPECT_GT(pool->size(), 0);
  }

  EXPECT_GE(pool->size(), 2);
}

TEST_F(ObjectPoolTest, HandlesStoredInContainers) {
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(3);
  std::vector<ObjectPoolManager<TrackingPerson>::Handle> handles;
  handles.reserve(3);

  for (int i = 0; i < 3; ++i) {
    handles.emplace_back(pool->construct("User" + std::to_string(i), i));
  }

  EXPECT_TRUE(pool->exhausted());

  handles.pop_back();
  EXPECT_EQ(pool->size(), 1);

  handles.clear();
  EXPECT_EQ(pool->size(), 3);
}

TEST_F(ObjectPoolTest, RejectsForeignPointer) {
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(1);
  auto fake = new TrackingPerson("Intruder", 99);

  EXPECT_THROW(pool->release(fake), std::invalid_argument);

  delete fake;
}

TEST_F(ObjectPoolTest, HighChurnStressTest) {
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(10);

  for (int i = 0; i < 1000; ++i) {
    auto obj = pool->construct("Stress", i);
  }

  EXPECT_EQ(TrackingPerson::constructions, TrackingPerson::destructions);
}

//
// TEST_F(ObjectPoolTest, ConcurrentAccess) {
//   auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(20);
//   std::vector<std::thread> threads;
//
//   for (int i = 0; i < 4; ++i) {
//     threads.emplace_back([pool, i]() {
//       for (int j = 0; j < 50; ++j) {
//         auto obj = pool->construct("Thread" + std::to_string(i), j);
//         std::this_thread::sleep_for(std::chrono::microseconds(1));
//       }
//     });
//   }
//
//   for (auto &t : threads) {
//     t.join();
//   }
//
//   EXPECT_GT(pool->size(), 0);
// }

TEST_F(ObjectPoolTest, NullptrHandling) {
  auto pool = std::make_shared<ObjectPoolManager<TrackingPerson>>(1);

  EXPECT_NO_THROW({ ObjectPoolManager<TrackingPerson>::Handle empty_handle; });
}
