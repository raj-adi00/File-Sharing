# cmake_minimum_required(VERSION 3.20)
# project(p2p LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(OPENSSL_INC "C:/Program Files/OpenSSL-Win64/include")
# set(OPENSSL_LIB "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MT/libcrypto.lib")

# add_executable(p2p
#     src/main.cpp
#     src/core/Logger.cpp
#     src/core/ConfigManager.cpp
#     src/core/PeerCore.cpp
#     src/discovery/DiscoveryService.cpp
#     src/net/TcpListener.cpp
#     src/net/TcpConnection.cpp
#     src/protocol/Message.cpp
#     src/protocol/MessageFramer.cpp
#     src/discovery/PeerTable.cpp
#     src/protocol/ProtocolSession.cpp
#     src/crypto/CryptoEngine.cpp
#     src/storage/FileManager.cpp
#     src/storage/ChunkManager.cpp
#     src/crypto/KeyExchange.cpp
#     src/storage/ResumeState.cpp
#     src/ui/ProgressTracker.cpp
#     src/ui/CLI.cpp
# )

# target_include_directories(p2p PRIVATE src ${OPENSSL_INC})

# if(WIN32)
#     target_link_options(p2p PRIVATE -static-libgcc -static-libstdc++ -static)
#     find_package(Threads REQUIRED)
#     target_link_libraries(p2p PRIVATE Threads::Threads ws2_32 ${OPENSSL_LIB} crypt32)
# endif()

cmake_minimum_required(VERSION 3.20)
project(p2p LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Path Configuration ---
set(OPENSSL_INC "C:/Program Files/OpenSSL-Win64/include")
set(OPENSSL_LIB "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MT/libcrypto.lib")

# The location where you placed your DLLs
set(DLL_SOURCE_DIR "D:/Projects/FileSharing/p2p")

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
    src/storage/ResumeState.cpp
    src/ui/ProgressTracker.cpp
    src/ui/CLI.cpp
)

target_include_directories(p2p PRIVATE src ${OPENSSL_INC})

if(WIN32)
    # 1. Handle C++ Runtime Static Linking
    # This prevents "msvcp140.dll" or "libstdc++-6.dll" missing errors on other PCs
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_link_options(p2p PRIVATE -static-libgcc -static-libstdc++ -static)
    endif()

    # 2. Link necessary libraries
    find_package(Threads REQUIRED)
    target_link_libraries(p2p PRIVATE 
        Threads::Threads 
        ws2_32 
        ${OPENSSL_LIB} 
        crypt32 
        advapi32
    )

    # 3. Automation: Copy DLLs and Config to the build folder automatically
    # This keeps everything in the same dir as p2p.exe for easy sharing
    add_custom_command(TARGET p2p POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_SOURCE_DIR}/libcrypto-3-x64.dll"
        "${DLL_SOURCE_DIR}/libssl-3-x64.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        "$<TARGET_FILE_DIR:p2p>"
        COMMENT "Gathering p2p.exe, DLLs, and config.ini into the build folder..."
    )
endif()