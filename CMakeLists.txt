cmake_minimum_required(VERSION 2.8)
project(ARTSynchronized)

if( NOT CMAKE_BUILD_TYPE )
    message(STATUS "No build type selected, default to Release")
    set( CMAKE_BUILD_TYPE Release)
else()
    message(STATUS "Build type is set to ${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra -march=native")

# cmake不能直接用$HOME，需要加ENV
# 设置jemalloc和TBB的根目录到CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/lib/jemalloc")
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/opt/oneTBB-2018_U6/build/linux_intel64_gcc_cc9_libc2.27_kernel4.15.0_release")
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/opt/oneTBB-2018_U6/include/tbb")

# 上面仅仅链接了库文件，并不能确保编译器知道库的头文件在哪里，所以还需要引入头文件的文件夹
include_directories("$ENV{HOME}/opt/oneTBB-2018_U6/include")

# set(JemallocLib "$ENV{HOME}/lib/jemalloc/lib/libjemalloc.so")
# list(APPEND TbbLib "$ENV{HOME}/opt/oneTBB-2018_U6/build/linux_intel64_gcc_cc9_libc2.27_kernel4.15.0_release/libtbbmalloc_proxy.so")
# list(APPEND TbbLib "$ENV{HOME}/opt/oneTBB-2018_U6/build/linux_intel64_gcc_cc9_libc2.27_kernel4.15.0_release/libtbbmalloc.so")
# list(APPEND TbbLib "$ENV{HOME}/opt/oneTBB-2018_U6/build/linux_intel64_gcc_cc9_libc2.27_kernel4.15.0_release/libtbb.so")

# find_package(TBB REQUIRED)
find_library(JemallocLib jemalloc)
find_library(TbbLib tbb)
find_package (Threads)
find_package (OpenMP REQUIRED)

set(ART_FILES OptimisticLockCoupling/Tree.cpp ROWEX/Tree.cpp ART/Tree.cpp)
add_library(ARTSynchronized ${ART_FILES})
target_link_libraries(ARTSynchronized ${TbbLib} ${JemallocLib} ${CMAKE_THREAD_LIBS_INIT} OpenMP::OpenMP_CXX) 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g")

set(SOURCE_FILES example.cpp)
add_executable(example ${SOURCE_FILES})
target_link_libraries(example ARTSynchronized)

# 这样的条件判断将会帮助你确认是否真的找到了库
if(JemallocLib)
    message(STATUS "Found jemalloc: ${JemallocLib}")
else()
    message(FATAL_ERROR "jemalloc not found")
endif()

if(TbbLib)
    message(STATUS "Found tbb: ${TbbLib}")
else()
    message(FATAL_ERROR "tbb not found")
endif()