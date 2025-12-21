# 🚀 PeerShare (P2P-File-Transfer)

### A high-performance, command-line utility for secure, **peer-to-peer file sharing** powered by a custom binary protocol.

PeerShare is designed for speed and reliability. By cutting out the middleman (central servers), it allows for direct, encrypted data exchange between peers on a local network or over the internet.

---

## ✨ Key Features (Current & Roadmap)

### 🛠️ Architecture & Core Logic (Implemented)
* **Custom Binary Protocol:** Uses a high-efficiency **Length-Prefix framing system** to ensure data integrity over TCP streams.
* **Config-Driven Design:** All system behaviors (ports, buffer sizes, security limits) are controlled via `config.ini`. Update the app without recompiling!
* **Thread-Safe Logging:** Multi-level logging (INFO, DEBUG, ERROR) to both console and persistent files for easy debugging.
* **Robust Error Handling:** Integrated "Memory Bomb" protection and packet validation to reject malformed data.

### 🌐 Networking & Security (In Progress)
* **TCP/IP Reliability:** Guaranteed delivery of file chunks using connection-oriented logic.
* **UDP Peer Discovery:** (Day 4) Automatic peer detection on the local network—no need to type IP addresses.
* **Binary Message Framing:** Custom headers including `MessageType` and `PayloadLength` for structured communication.

---

## 🏗️ The Protocol: How it works

PeerShare doesn't just send raw text; it uses a structured binary format to ensure that even large files arrive without a single bit out of place.



| Field | Size | Description |
| :--- | :--- | :--- |
| **Type** | 2 Bytes | Identifies the message (HELLO, PING, FILE_DATA, etc.) |
| **Length** | 4 Bytes | The size of the incoming payload (supports up to 4GB) |
| **Payload** | Variable | The actual data (encrypted file chunks or system commands) |

---

## ⚙️ Quick Start: Build and Run

### Prerequisites
* **C++17 Compiler** (MSVC, MinGW, or Clang)
* **CMake** 3.10+
* **Windows Socket API** (Winsock2)

### Installation
```bash
# Clone the repository
git clone '[https://github.com/raj-adi00/File-Sharing.git](https://github.com/raj-adi00/File-Sharing.git)'
cd p2p

# Clean up any existing build folder
rm -rf build

# Configure and Build
cmake -S . -B build
cmake --build build

# Start the P2P file sharing platform Server on Terminal 1
build/p2p.exe Server
#Start the P2P file sharing platform Client on Terminal 2
build/p2p.exe Client
