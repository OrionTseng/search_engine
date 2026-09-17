cmake_minimum_required(VERSION 3.10)
project(search_engine)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 查找所有源文件
file(GLOB SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/KeyRecommander/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LRUCache/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Reactor/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SearchEngineServer/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TimerManager/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WebPageSearcher/*.cc
)

# 设置第三方库头文件目录
# json是一个存头文件第三方库，将其复制到系统默认目录即可不用在CMakeLists.txt中配置
# 在系统默认目录下的第三库都不需要在CMakeLists.txt中配置，使用自定义目录时需要进行配置
set(LOG4CPP_HEAD "/usr/include/log4cpp/" )

# 设置第三方库文件目录
set(LOG4CPP_PATH "/usr/lib/aarch64-linux-gnu/")

# 添加可执行文件
add_executable(SearchEngine ${SRC})

# 包含头文件目录
target_include_directories(SearchEngine PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/KeyRecommander
    ${CMAKE_CURRENT_SOURCE_DIR}/include/LRUCache
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Reactor
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SearchEngineServer
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TimerManager
    ${CMAKE_CURRENT_SOURCE_DIR}/include/WebPageSearcher
    ${CMAKE_CURRENT_SOURCE_DIR}/include/WebPageSearcher/cppjieba
    ${LOG4CPP_HEAD}
    ${LOG4CPP_PATH}
)

# 连接第三方库
# 如果第三方库有静态/动态库一定要进行链接
target_link_libraries(SearchEngine PRIVATE 
    log4cpp
)
