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
#include <vector>

using size_type = std::size_t;

template <typename T>

class ObjectPoolManager
    : public std::enable_shared_from_this<ObjectPoolManager<T>> {

private:
  // A vector containing multiple <T> pointers (multiple <T  objects)
  std::vector<T *> free_list;

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

      // If the pool is still alive, just call 'release' method that put the
      // pointer into the free_list
      if (auto p = weak_ptr_pool.lock()) {

        // Calling the deconstructor for the specific obj
        p->release(ptr);

      }

      else

        // Delete the pointer to the array
        // The MemoryPoolManager does not exists, because `.lock()` returns
        // nullptr
        delete ptr;
    }
  };

  // The "wrapper" for returning a T* into a std::unique_ptr through interface
  using Handle = std::unique_ptr<T, PoolCustomDeleter>;

  ObjectPoolManager();

  ObjectPoolManager(size_type num_blocks);

  // The following method try to acquire a resource, pointed by T*,
  // and return a `std::unique_ptr` where it incapsulate the raw pointer
  //
  // The raw pointer is also removed from the free-list
  Handle acquire();

  bool is_empty() const;

  size_type size() const;

  void release(T *ptr);

  template <typename... Args> Handle construct(Args &&...args) {

    T *ptr = this->acquire_raw();

    ::new (ptr) T(args...);

    PoolCustomDeleter customD;

    customD.weak_ptr_pool = this->shared_from_this();

    return Handle(ptr, customD);
  }
  ~ObjectPoolManager();
};

template <typename T>
ObjectPoolManager<T>::ObjectPoolManager(size_type num_blocks) {

  // Assertion about the two constructor parameters
  assert(num_blocks >= 0);

  for (size_type i = 0; i < num_blocks; i++) {
    T *ptr = new T();

    // For debugging
    std::cout << "Pointer address: " << ptr << std::endl;

    this->free_list.push_back(ptr);
  }

  std::cout << "Allocated " << num_blocks << " blocks" << std::endl;
}

template <typename T> ObjectPoolManager<T>::~ObjectPoolManager() {

  std::cout << "Deleting the memory pool manager..." << std::endl;
  for (auto ptr : this->free_list)
    delete ptr;
}

template <typename T>
typename ObjectPoolManager<T>::Handle ObjectPoolManager<T>::acquire() {

  assert(!this->is_empty());

  // Assign to `ptr` the last element of the free list
  T *ptr = this->free_list.back();

  // Remove from the list with all blocks available the last one
  this->free_list.pop_back();

  // Create a new CustomDeleter for each resource given by `.acuire()`
  // Every new resource must have a own custom deleter with
  PoolCustomDeleter customD;

  // shared_from_this = create a new shared_ptr that is linked to the same
  // object of 'this'
  // this shared pointer is memorize into a weak_ptr of the Deleter
  // The `shared_from_this()` return a shared_ptr() that points to the same
  // object pointed by `this`
  customD.weak_ptr_pool = this->shared_from_this();

  // Return a new Handle with the ptr wrapped and a new custom deleter
  return Handle(ptr, customD);
}

// The following function is called when the std::unique_ptr acquire by
// the user become out-of-scope
//
// This metod must be used only by PoolCustomDeleter

template <typename T> void ObjectPoolManager<T>::release(T *ptr) {

  this->free_list.push_back(ptr);

  std::cout << "The resource is just released and push back into free-list!"
            << std::endl;
}

template <typename T> bool ObjectPoolManager<T>::is_empty() const {
  return this->free_list.empty();
}

template <typename T> size_type ObjectPoolManager<T>::size() const {
  return this->free_list.size();
}

#endif
