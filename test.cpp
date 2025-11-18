#include <iostream>
#include "MemoryPoolManager.hh";

int main() {


    MemoryPoolManager mp1(1024, 10); // Allocating 10 blocks of 1024 size for each one
    

    auto h = mp1.acquire(); // requiring a block and save it into `h`
    


    return 0;
}
