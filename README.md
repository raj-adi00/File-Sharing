# 🚀 PeerShare (P2P-File-Transfer)

### A high-performance, command-line utility for secure, **peer-to-peer file sharing** powered by a custom binary protocol.

PeerShare is designed for speed and reliability. By cutting out the middleman (central servers), it allows for direct, encrypted data exchange between peers on a local network or over the internet.

---

## ✨ Key Features (Current & Roadmap)

### 🛠️ Architecture & Core Logic (Implemented)
* **Custom Binary Protocol:** High-efficiency **Length-Prefix framing system** that ensures data integrity and prevents packet fragmentation.
* **UDP Peer Discovery (New!):** Automatic detection of peers on the local network using UDP broadcasting. No manual IP entry required.
* **Dynamic Peer Table:** Real-time, thread-safe registry of active neighbors discovered on the LAN.
* **Config-Driven Design:** System behaviors (discovery ports, broadcast intervals, log paths) are managed via `config.ini`. Update settings without recompiling.
* **Multi-Threaded Engine:** Parallel execution of Discovery (UDP), Listeners (TCP), and the Main UI/Logic thread.
* **Robust Logging:** Multi-level (INFO, DEBUG, ERROR) thread-safe logging to both console and `p2p.log`.

### 🌐 Networking & Security (In Progress - Day 5)
* **Protocol Handshake:** Implementation of `HELLO` and `HELLO_ACK` exchange to verify peer identity and capabilities before data transfer.
* **Session Management:** Tracking active TCP connections and protocol versions to ensure compatibility between nodes.
* **Heartbeat & Liveness:** Integrating "Last Seen" logic to automatically prune inactive peers from the table.
* **AES-256 Encryption:** (Future) Planned integration of secure payload encryption for private file sharing.

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
build/p2p.exe
#Start the P2P file sharing platform Client on Terminal 2
build/p2p.exe
