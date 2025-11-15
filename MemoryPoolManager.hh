#include <iostream>


using size_type = size_t;
template<typename T>

class MemoryPoolManager {

    private:
        std::vector<T> free_list;
        using Handle = std::unique_ptr<T>;

    public:
        MemoryPoolManager();

        MemoryPoolManager(size_type dim_block, size_type num_blocks);
        
        // The following method try to acquire a resource, pointed by T*,
        // and return a `std::unique_ptr` where it incapsulate the raw pointer
        //
        // The raw pointer is also removed from the free-list
        Handle acquire();

        bool is_empty() const;

        ~MemoryPoolManager();


};
