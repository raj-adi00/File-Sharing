cmake_minimum_required(VERSION 3.20)
project(p2p LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENSSL_INC "C:/Program Files/OpenSSL-Win64/include")
set(OPENSSL_LIB "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MT/libcrypto.lib")

add_executable(p2p
    src/main.cpp
    src/core/Logger.cpp
    src/core/ConfigManager.cpp
    src/core/PeerCore.cpp
    src/discovery/DiscoveryService.cpp
    src/net/TcpListener.cpp
    src/net/TcpConnection.cpp
    src/protocol/Message.cpp
    src/protocol/MessageFramer.cpp
    src/discovery/PeerTable.cpp
    src/protocol/ProtocolSession.cpp
    src/crypto/CryptoEngine.cpp
    src/storage/FileManager.cpp
    src/storage/ChunkManager.cpp
    src/crypto/KeyExchange.cpp
)

target_include_directories(p2p PRIVATE src ${OPENSSL_INC})

if(WIN32)
    target_link_options(p2p PRIVATE -static-libgcc -static-libstdc++ -static)
    find_package(Threads REQUIRED)
    target_link_libraries(p2p PRIVATE Threads::Threads ws2_32 ${OPENSSL_LIB} crypt32)
endif()