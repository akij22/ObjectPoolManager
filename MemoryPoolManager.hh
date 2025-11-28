#ifndef MEMORY_POOL_MANAGER
#define MEMORY_POOL_MANAGER

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

using size_type = std::size_t;

template <typename T>

class MemoryPoolManager
    : public std::enable_shared_from_this<MemoryPoolManager<T>> {

private:
  // A vector containing multiple <T> pointers
  std::vector<T *> free_list;

public:
  // Definition of a struct custom deleter for Handle
  struct PoolCustomDeleter {
    std::weak_ptr<MemoryPoolManager<T>> weak_ptr_pool;

    // When Handle become out-of-scope, the operator() is called
    void operator()(T *ptr) {

      if (!ptr)
        throw;

      // If the pool is still alive, just call 'release' method that put the
      // pointer into the free_list
      if (auto p = weak_ptr_pool.lock())
        p->release(ptr);

      else

        // If the pool does not exists yet, just delete the pointer
        delete ptr;
    }
  };

  // The "wrapper" for returning a T* into a std::unique_ptr through interface
  using Handle = std::unique_ptr<T, PoolCustomDeleter>;

  MemoryPoolManager();

  MemoryPoolManager(size_type dim_block, size_type num_blocks);

  // The following method try to acquire a resource, pointed by T*,
  // and return a `std::unique_ptr` where it incapsulate the raw pointer
  //
  // The raw pointer is also removed from the free-list
  Handle acquire();

  bool is_empty() const;

  size_type size() const;

  size_type available() const;
  void release(T *ptr);

  ~MemoryPoolManager();
};

template <typename T>
MemoryPoolManager<T>::MemoryPoolManager(size_type dim_block,
                                        size_type num_blocks) {

  for (size_type i = 0; i < num_blocks; i++) {
    T *ptr = new T[dim_block];

    this->free_list.push_back(ptr);
  }

  std::cout << "Allocated " << num_blocks << " blocks" << std::endl;
}

template <typename T> MemoryPoolManager<T>::~MemoryPoolManager() {

  std::cout << "Deleting the memory pool manager..." << std::endl;
  for (auto ptr : this->free_list)
    delete ptr;
}

template <typename T>
typename MemoryPoolManager<T>::Handle MemoryPoolManager<T>::acquire() {

  assert(!this->is_empty());

  // Assign to `ptr` the last element of the free list
  T *ptr = this->free_list.back();

  // Remove from the list with all blocks available the last one
  this->free_list.pop_back();

  // Return a pointer incapsulated into a `std::unique_ptr`

  PoolCustomDeleter customD;

  // shared_from_this = create a new shared_ptr that is linked to the same
  // object of 'this'
  // this shared pointer is memorize into a weak_ptr of the Deleter
  customD.weak_ptr_pool = this->shared_from_this();

  // Return a new Handle with the ptr wrapped and a new custom deleter
  return Handle(ptr, customD);
}

// The following function is called when the std::unique_ptr acquire by
// the user become out-of-scope
//
// This metod must be used only by PoolCustomDeleter

template <typename T> void MemoryPoolManager<T>::release(T *ptr) {

  this->free_list.push_back(ptr);

  std::cout << "The resource is just released and push back into free-list!"
            << std::endl;
}

template <typename T> bool MemoryPoolManager<T>::is_empty() const {
  return this->free_list.empty();
}

template <typename T> size_type MemoryPoolManager<T>::size() const {
  return this->free_list.size();
}

// Return the number of blocks available for acquiring
template <typename T> size_type MemoryPoolManager<T>::available() const {
  return this->free_list.capacity() - this->free_list.size();
}

#endif
