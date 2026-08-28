cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

# VCPKG
# cmake -DCMAKE_TOOLCHAIN_FILE=/opt/workspace/vcpkg/scripts/buildsystems/vcpkg.cmake ..
# Needed libs are in file vcpkg.json
# Windows required libs: .\vcpkg install --triplet x64-windows asio pugixml spdlog curl protobuf parallel-hashmap magic-enum mio luajit libmariadb mpir abseil bshoshany-thread-pool

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

set(VCPKG_FEATURE_FLAGS "versions")
set(VCPKG_BUILD_TYPE "release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# *****************************************************************************
# Project crystalserver
# *****************************************************************************
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    project(crystalserver-debug LANGUAGES CXX)
else()
    project(crystalserver LANGUAGES CXX)
endif()

# *****************************************************************************
# Append cmake search path
# *****************************************************************************
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# The oldest CPU the shipped binary must run on. Set it here, once, so the same
# source produces the same binary no matter which machine compiled it -- never
# -march=native, which silently ties the binary to the build host.
#
#   x86-64-v2          SSE4.2, POPCNT             (Nehalem/Bulldozer, ~2009+)
#   x86-64-v3          AVX2, BMI, FMA             (Haswell/Excavator, ~2015+; typical VPS)
#   armv8.2-a+crypto   Graviton2+, Ampere Altra
#   <empty>            the compiler's own default
#
# Leave it empty on Apple Silicon. Apple clang already targets the host CPU tuple
# there, and forcing -march= drops it onto a generic armv8.x profile that loses
# the AES, SHA2/3/512 and FP16 extensions the default target has.
set(CRYSTAL_ARCH_BASELINE "" CACHE STRING
    "Oldest CPU the binary must run on, as an -march= value. Empty = compiler default.")

if(NOT MSVC AND CRYSTAL_ARCH_BASELINE)
    add_compile_options(-march=${CRYSTAL_ARCH_BASELINE})
endif()

# *****************************************************************************
# Include cmake tools
# *****************************************************************************
include(MessageColors)
include(LoggingHelper)

# *****************************************************************************
# Options
# *****************************************************************************
option(OPTIONS_ENABLE_CCACHE "Enable ccache" OFF)
option(OPTIONS_ENABLE_SCCACHE "Use sccache to speed up compilation process" OFF)
option(OPTIONS_ENABLE_IPO "Check and Enable interprocedural optimization (IPO/LTO)" ON)
option(FEATURE_METRICS "Enable metrics feature" OFF)

# *****************************************************************************
# Options Code
# *****************************************************************************

if(FEATURE_METRICS)
    log_option_enabled("metrics")
else ()
    log_option_disabled("metrics")
endif ()

# === CCACHE ===
if(OPTIONS_ENABLE_CCACHE)
    find_program(CCACHE ccache)
    if(CCACHE)
        log_option_enabled("ccache")
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    else()
        log_option_disabled("ccache")
    endif()
endif()

# === SCCACHE ===
if(OPTIONS_ENABLE_SCCACHE)
    find_program(SCCACHE_PATH sccache)
    if(SCCACHE_PATH)
        log_option_enabled("sccache")
        set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_PATH})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_PATH})
    else()
        log_option_disabled("sccache")
    endif()
endif()

option(BUILD_TESTS "Build tests" OFF) # By default, tests will not be built
option(RUN_TESTS_AFTER_BUILD "Run tests when building" OFF) # By default, tests will only run if requested

# *****************************************************************************
# Add project
# *****************************************************************************
add_subdirectory(src)

if(BUILD_TESTS OR PACKAGE_TESTS)
    # Must be called here, not only in tests/CMakeLists.txt. CTest writes a
    # CTestTestfile.cmake into the directory where enable_testing() runs and into
    # its children; called only from the subdirectory, nothing is generated at the
    # build root, so `ctest` run from there reports "No tests were found" and exits
    # non-zero even though the suites built fine.
    enable_testing()
    add_subdirectory(tests)
endif()
