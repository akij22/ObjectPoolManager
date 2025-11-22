# Repository Guidelines

## Project Structure & Module Organization
The project stays intentionally lean: `MemoryPoolManager.hh` exposes the templated interface and handle wrapper, `MemoryPoolManager.cpp` owns allocation logic, and `test.cpp` drives a manual usage scenario. Keep reusable declarations in headers, isolate implementation details per translation unit, and add future experiments or perf harnesses under `samples/` while growing dedicated specs in `tests/`.

## Build, Test, and Development Commands
- `mkdir -p build && g++ -std=c++20 -Wall -Wextra -pedantic MemoryPoolManager.cpp test.cpp -o build/memory_pool_demo`: compile the pool plus the demo test target; append `-g` or sanitizers while debugging allocator behavior.
- `./build/memory_pool_demo`: run the executable to verify acquisitions/releases and print diagnostic output.
- `clang-format -i MemoryPoolManager.hh MemoryPoolManager.cpp test.cpp`: normalize style before reviews; the LLVM profile keeps headers and definitions consistent.

## Coding Style & Naming Conventions
Use the c++11 standard, four-space indentation, and place braces on new lines for class or free-function definitions. Classes/structs follow `PascalCase` (`MemoryPoolManager`), methods and free helpers use `snake_case` (`acquire`, `is_empty`), and members remain descriptive (`free_list`). Favor `const` correctness and explicit ownership semantics—template code should live entirely in headers when needed externally. Prefer exceptions or status returns over bare `throw;` once behaviors solidify.

## Testing Guidelines
Current coverage is a single demo `main` inside `test.cpp`. Add focused cases to `tests/<feature>_test.cpp`, drive them with assertions or targeted logging, and extend the build command with every new file. Always rerun the standard build plus a sanitizer build (`-fsanitize=address,undefined`) before opening a PR, and document allocator invariants directly in the test description.

## Commit & Pull Request Guidelines
History follows Conventional Commits (`feat(MemoryPoolManager): …`, `fix: …`). Start each commit with a lowercase type, optional scope, and a concise summary of the change. PRs must explain intent, list validation commands, link issues, and call out API impacts (e.g., changes to `Handle` semantics). Include screenshots or logs only when they clarify allocator behavior, and confirm handles are safely released before requesting review.

## Resource Safety Tips
Maintain clear ownership rules: every `Handle` returned by `acquire` must eventually go out of scope or pass through `release`. Avoid sharing raw pointers without documenting who releases them, and gate future concurrency work with mutexes around `free_list` operations.
