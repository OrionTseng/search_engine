cmake_minimum_required(VERSION 3.10)
project(word)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 搜索对应的源文件
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

# 添加可执行文件
add_executable(word ${SRC})

# 包含头文件目录
target_include_directories(word PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cppjieba 
)