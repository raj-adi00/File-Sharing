# 🚀 PeerShare

### An efficient, command-line utility for secure, **peer-to-peer file sharing** that requires no central server.

---

## ⚙️ Quick Start: Build and Run

Follow these simple steps to clone the repository, compile the application using `cmake`, and run the executable.

### 1. Clone the Repository

Navigate to where you want to store the project and clone the files:

```bash
git clone [https://github.com/raj-adi00/File-Sharing.git]
cd p2p
# Clean up any existing build folder
rm -rf build 

# Configure the build system (creates the 'build' directory)
cmake -S . -B build

# Compile the project
cmake --build build

# Start the P2P file sharing platform
build/p2p.exe
```
