cmake_minimum_required(VERSION 3.14)
project(ObjectPoolManager VERSION 1.0 LANGUAGES CXX)

# IMPORTANTE: Abilita export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flag di compilazione
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# ========== GOOGLE TEST ==========

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ========== TARGET DEI TEST ==========

# Crea l'eseguibile dei test
add_executable(pool_tests test_object_pool.cpp)

# Include directory (per trovare ObjectPoolManager.hh)
target_include_directories(pool_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link con Google Test
target_link_libraries(pool_tests PRIVATE GTest::gtest_main)

# Scopri test automaticamente
include(GoogleTest)
gtest_discover_tests(pool_tests)