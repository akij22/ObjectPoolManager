/*
 * This templatic class represent a memory pool, where each single cell is a T
 * object.
 *
 * Acquiring with `.acquire()` must return a unique_ptr to a single T object
 *
 * */

#ifndef OBJECT_POOL_MANAGER
#define OBJECT_POOL_MANAGER

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <vector>

using size_type = std::size_t;

struct Stats {

  size_type totalBlocks;
  size_type usedBlocks;
  size_type freeBlocks;
  size_type allocationCount;
  size_type deallocationCount;
};

template <typename T>

class ObjectPoolManager
    : public std::enable_shared_from_this<ObjectPoolManager<T>> {

private:
  // A vector containing multiple <T> pointers (multiple <T  objects)
  std::vector<T *> free_list;

  std::unordered_set<T *> pool_pointers;
  Stats stats;

  // return a T raw pointer for creating object into memory already allocated
  T *acquire_raw() {

    T *ptr = this->free_list.back();

    this->free_list.pop_back();

    return ptr;
  }

public:
  // Definition of a struct custom deleter for Handle
  struct PoolCustomDeleter {
    std::weak_ptr<ObjectPoolManager<T>> weak_ptr_pool;

    // When Handle become out-of-scope, the operator() is called
    void operator()(T *ptr) {

      if (!ptr)
        throw;

      // Delete the object build with `.construct()` during acquiring
      ptr->~T();

      // If the pool is still alive, just call 'release' method that put the
      // pointer into the free_list
      if (auto p = weak_ptr_pool.lock()) {

        // Calling the deconstructor for the specific obj
        p->release(ptr);
      }

      else

        // If the object pool is not alive yet, also delete the raw memory
        ::operator delete(ptr, std::align_val_t(alignof(T)));
    }
  };

  // The "wrapper" for returning a T* into a std::unique_ptr through interface
  using Handle = std::unique_ptr<T, PoolCustomDeleter>;

  ObjectPoolManager();
  ~ObjectPoolManager();

  ObjectPoolManager(size_type num_blocks);

  // The following method try to acquire a resource, pointed by T*,
  // and return a `std::unique_ptr` where it incapsulate the raw pointer
  //
  // The raw pointer is also removed from the free-list
  // Handle acquire();

  bool exhausted() const;

  size_type size() const;

  void release(T *ptr);

  // The `.construct` method take n parameters
  template <typename... Args> Handle construct(Args... args) {

    T *ptr = this->acquire_raw();

    // Build the object in the ptr memeory location (placement new)
    // We have the access to this new object created through ptr pointer
    new (ptr) T(args...);

    PoolCustomDeleter customD;

    customD.weak_ptr_pool = this->shared_from_this();

    std::cout << "DEBUG: Returning pointer with address: " << ptr << std::endl;
    // Update stats
    --stats.freeBlocks;
    ++stats.usedBlocks;

    // Build a Handle with the pointer to the new object and the custom deleter
    return Handle(ptr, customD);
  }
};

template <typename T>
ObjectPoolManager<T>::ObjectPoolManager(size_type num_blocks) {

  // Assertion about the two constructor parameters
  assert(num_blocks >= 0);

  for (size_type i = 0; i < num_blocks; i++) {

    // Allocating new raw memory without building any obect
    void *ptr_raw = ::operator new(sizeof(T), std::align_val_t(alignof(T)));

    // Casting from raw memory pointer to T* ptr
    T *ptr = static_cast<T *>(ptr_raw);

    this->pool_pointers.insert(ptr);

    // For debugging
    std::cout << "Pointer address: " << ptr << std::endl;

    this->free_list.push_back(ptr);
  }

  // Initialization of stats
  stats.totalBlocks = num_blocks;
  stats.freeBlocks = num_blocks;
  stats.allocationCount = num_blocks;
  std::cout << "DEBUG: Allocated " << stats.totalBlocks << " blocks"
            << std::endl;
}

template <typename T> ObjectPoolManager<T>::~ObjectPoolManager() {

  // TODO Check what pointers are inside pool_pointers

  for (T *elem : this->free_list) {

    // Delete manually the raw memory

    std::cout << "DEBUG(DECONSTRUCTOR), deleting pointer to raw memory: "
              << elem << std::endl;

    ::operator delete(elem, std::align_val_t(alignof(T)));

    ++stats.deallocationCount;
  }
}

// template <typename T>
// typename ObjectPoolManager<T>::Handle ObjectPoolManager<T>::acquire() {
//
//   assert(!this->is_empty());
//
//   // Assign to `ptr` the last element of the free list
//   T *ptr = this->free_list.back();
//
//   // Remove from the list with all blocks available the last one
//   this->free_list.pop_back();
//
//   // Create a new CustomDeleter for each resource given by `.acquire()`
//   // Every new resource must have a own custom deleter with
//   PoolCustomDeleter customD;
//
//   /* shared_from_this = create a new shared_ptr that is linked to the same
//    *  object of 'this'
//    * this shared pointer is memorize into a weak_ptr of the Deleter
//    * The `shared_from_this()` return a shared_ptr() that points to the same
//    * object pointed by `this`
//    customD.weak_ptr_pool = this->shared_from_this();
//
//    Return a new Handle with the ptr wrapped and a new custom deleter
//    return Handle(ptr, customD);
//
//   */
// }
//
// The following function is called when the std::unique_ptr acquire by
// the user become out-of-scope
//
// This metod must be used only by PoolCustomDeleter

template <typename T> void ObjectPoolManager<T>::release(T *ptr) {

  // If the ptr parameter is not found into pool_pointers
  if (this->pool_pointers.find(ptr) == this->pool_pointers.end())
    throw std::invalid_argument(
        "The pointer does not belong to the object pool");

  this->free_list.push_back(ptr);

  std::cout << "The resource is just released and push back into free-list!"
            << std::endl;

  --stats.usedBlocks;
  ++stats.freeBlocks;
}

template <typename T> bool ObjectPoolManager<T>::exhausted() const {

  // Return true if there are no more slot available
  return this->free_list.empty();
}

template <typename T> size_type ObjectPoolManager<T>::size() const {
  return this->free_list.size();
}

#endif
