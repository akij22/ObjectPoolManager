#include <iostream>


using size_type = size_t;


template<typename T>

class MemoryPoolManager {

    private:

        // A vector containing multiple <T> pointers
        std::vector<T*> free_list;
            public:

        // Definition of a struct custom deleter for Handle
        struct PoolCustomDeleter {

            std::weak_ptr<MemoryPoolManager<T>> weak_ptr_pool;
            void operator()(T* ptr) {
    
                if (!ptr) throw;

                if (auto p = weak_ptr_pool.lock())
                    p->release(ptr);

                else delete p;
            }

        };

        using Handle = std::unique_ptr<T, PoolCustomDeleter>;

        MemoryPoolManager();

        MemoryPoolManager(size_type dim_block, size_type num_blocks);
        
        // The following method try to acquire a resource, pointed by T*,
        // and return a `std::unique_ptr` where it incapsulate the raw pointer
        //
        // The raw pointer is also removed from the free-list
        Handle acquire();

        bool is_empty() const;
        
        
        void release(T* ptr);

        ~MemoryPoolManager();

};


template<typename T>


