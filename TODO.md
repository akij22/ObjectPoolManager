# TODO

- [ ] Implement `ObjectPoolManager<T>::acquire` so it actually configures `PoolCustomDeleter::weak_ptr_pool` and returns a `Handle` instead of popping from `free_list` and doing nothing (`ObjectPoolManager.hh:136-161`).
- [ ] Either define the declared default constructor or drop it so the class cannot be instantiated in an unusable state (`ObjectPoolManager.hh:73`).
- [ ] Replace the assert-only guards in `acquire_raw`, `acquire`, and `construct` with runtime handling (throw or grow the pool) when `free_list` is empty; relying on `assert(!is_empty())` causes undefined behavior once assertions are compiled out (`ObjectPoolManager.hh:33-40`, `92-103`, `136-139`).
- [ ] Ensure object destructors run before memory is recycled: let `PoolCustomDeleter::operator()` ignore `nullptr` instead of executing `throw;`, and have `release` invoke `ptr->~T()` before the pointer is pushed back into the pool (`ObjectPoolManager.hh:48-67`, `168-177`).
- [ ] Track which pointers are currently checked out (e.g., with another `unordered_set`) so `release` can reject double frees instead of silently inserting duplicates into `free_list` (`ObjectPoolManager.hh:168-177`).
- [ ] Update `construct` to take forwarding references and use `std::forward` so move-only types or expensive constructor arguments are supported without extra copies (`ObjectPoolManager.hh:89-104`).
- [ ] Turn the `testObjPool.cpp` demo into real assertions: stop releasing a foreign pointer without catching the resulting exception and add cases that validate successful acquire/construct/release and destructor invocation (`testObjPool.cpp:30-52`).
