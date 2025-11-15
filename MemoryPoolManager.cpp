#include <iostream>
#include "MemoryPoolManager.hh"

template<typename T>
MemoryPoolManager<T>::MemoryPoolManager(size_type dim_block, size_type num_blocks){
    
    for (size_type i = 0; i < num_blocks; i++){
        T* ptr = new T[dim_block];

        this->free_list.push_back(ptr);
    }


}
template<typename T>
MemoryPoolManager<T>::~MemoryPoolManager() {

    for (auto ptr : this->free_list)
        delete ptr;
}


template<typename T>
typename MemoryPoolManager<T>::Handle MemoryPoolManager<T>::acquire() {

    // Check if there is no resource to give by interface
    if (this->free_list.empty())
        
        // TEMP CODE: decide what to do if there is no resource available
        throw;

    
    // Assign to `ptr` the last element of the free list
    T* ptr = this->free_list.back();
    
    this->free_list.pop_back();

    // Return a pointer incapsulated into a `std::unique_ptr`
    return Handle(ptr);
}

