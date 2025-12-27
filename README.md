# Object Pool Manager

<u>This object pool manager exists to amortize expensive heap allocations by recycling raw, properly aligned storage for frequently constructed objects so latency-sensitive systems can hydrate new instances without touching the global allocator each time.</u>

A header-first allocator experiment centered on `ObjectPoolManager<T>`, which owns a reservoir of raw `T` slots, hands them out through RAII handles, and tracks usage statistics so you can reason about allocation churn. The pool preallocates aligned blocks via `::operator new`, constructs objects in-place with placement new, and returns memory automatically when handles go out of scope.

## Highlights & Features
- **Type-aware pool with placement new** – `construct(args...)` builds `T` directly inside preallocated storage and returns a `std::unique_ptr<T, PoolCustomDeleter>` handle.
- **Deterministic recycling & safety checks** – released pointers are validated against `pool_pointers` to prevent foreign memory from contaminating the pool, while the custom deleter guards double frees and pool teardown.
- **On-demand expansion** – pools start with a fixed capacity but `expand()` doubles the storage (or uses a caller-provided size) whenever `construct` would otherwise exhaust the free list.
- **Runtime stats** – the embedded `Stats` struct monitors total/used/free blocks along with allocation/deallocation counters so you can log health or drive metrics dashboards.
- **Test harnesses** – `testObjPool.cpp` offers a portable assertion-based suite, and `test_object_pool.cpp` contains the same scenarios expressed with GoogleTest (stress, container ownership, invalid releases, etc.).

## Repository Layout
- `ObjectPoolManager.hh` – templated interface, RAII handle, stats bookkeeping, and implementation.
- `testObjPool.cpp` – standard-library-only driver covering acquisition/release and pointer validation.
- `test_object_pool.cpp` – GoogleTest suite with higher-level checks and future concurrency experiments.
- `expand_on_demand_test.cpp`, `test.cpp` – older experiments kept for reference while iterating on allocator behavior.
- `samples/`, `tests/` (future) – add targeted repros or benchmarks without cluttering the main demo target.

## Getting Started
### Requirements
- C++20-capable compiler (GCC, Clang, or MSVC)
- Standard library headers; GoogleTest if you plan to run `test_object_pool.cpp`

### Build & run the portable demo
```bash
mkdir -p build && g++ -std=c++20 -Wall -Wextra -pedantic testObjPool.cpp -o build/object_pool_demo
./build/object_pool_demo
```

### Build the GoogleTest suite (requires gtest)
```bash
mkdir -p build && g++ -std=c++20 -Wall -Wextra -pedantic test_object_pool.cpp -lgtest -lpthread -o build/object_pool_gtest
./build/object_pool_gtest
```

### Sanitizer build (recommended)
```bash
mkdir -p build && g++ -std=c++20 -Wall -Wextra -pedantic -fsanitize=address,undefined testObjPool.cpp -o build/object_pool_demo
ASAN_OPTIONS=detect_leaks=1 ./build/object_pool_demo
```

### Formatting
```bash
clang-format -i ObjectPoolManager.hh testObjPool.cpp test_object_pool.cpp
```

## Usage Example
```cpp
#include "ObjectPoolManager.hh"
#include <memory>

struct Particle {
    float x{0};
    float y{0};
    Particle(float init_x, float init_y) : x(init_x), y(init_y) {}
};

int main() {
    auto pool = std::make_shared<ObjectPoolManager<Particle>>(4);

    {
        auto handle = pool->construct(1.0f, 2.0f); // placement-new inside pool
        handle->x += 5.0f;                         // use like a regular object
    } // PoolCustomDeleter destroys Particle and returns storage to free_list

    if (pool->exhausted()) {
        pool->expand(); // double the available slots
    }

    return 0;
}
```
Because `ObjectPoolManager` inherits from `std::enable_shared_from_this`, every handle stores a weak pointer back to the pool. When the pool still exists, the deleter destroys the object, validates the pointer, bumps stats, and pushes it onto `free_list`. If the pool has already been destroyed, the deleter falls back to `::operator delete` so no raw memory leaks.

## Stats at a Glance
`Stats` exposes `totalBlocks`, `usedBlocks`, `freeBlocks`, `allocationCount`, and `deallocationCount`. You can tap into it (e.g., via a future accessor) to detect churn, alert on exhaustion, or feed telemetry to profilers.

## Roadmap Ideas
1. Surface a const accessor for `Stats` plus structured logging hooks for diagnostics.
2. Add thread-safety (mutex or lock-free freelist) before sharing pools across workers.
3. Support configurable growth policies so large `T` types do not double blindly on `expand()`.
4. Provide adapters for integrating the pool into existing game/server engines or ECS frameworks.

## Contributing
Open issues or PRs for allocator tweaks, docs, or new tests. Follow Conventional Commits (`feat(object-pool): ...`, `fix: ...`) and document validation commands (build, run, sanitizer) in every PR. Ensure any `Handle` you acquire in samples or specs is released (or naturally destroyed) before asking for review to keep ownership semantics airtight.

