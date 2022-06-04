cmake_minimum_required(VERSION 3.14)
project(ClassesProject)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread,thread")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_DEBUG_LIBS         ON)
set(Boost_USE_RELEASE_LIBS      OFF) 
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF) 

#find_package(Boost REQUIRED COMPONENTS date_time filesystem system ...)
find_package(Boost 1.74.0)

if(Boost_FOUND)   
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(Test tests.cpp)   
    target_link_libraries(Test gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(Test)
endif()



