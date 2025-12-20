cmake_minimum_required(VERSION 3.20)
project(p2p LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(p2p
    src/main.cpp
    src/core/Logger.cpp
    src/core/ConfigManager.cpp
    src/core/PeerCore.cpp
    src/discovery/DiscoveryService.cpp
)

target_include_directories(p2p PRIVATE src)

if(WIN32)
    target_link_options(p2p PRIVATE -static-libgcc -static-libstdc++ -static)
    find_package(Threads REQUIRED)
    target_link_libraries(p2p PRIVATE Threads::Threads)
endif()