# 🚀 PeerShare (P2P File Sharing System in C++)

### A high-performance, secure, peer-to-peer file sharing system built from scratch in **C++**, using a custom binary protocol, automatic LAN peer discovery, encrypted communication, and resumable file transfers.

PeerShare enables **direct file exchange between peers without any central server**, focusing on reliability, integrity, and real-world networking design.

---

## 📌 Project Overview

PeerShare is a systems-level networking project demonstrating:
- Custom protocol design
- Concurrent TCP/UDP networking
- Secure and verified file transfer
- Fault tolerance and recovery

The system is fully **CLI-based** and **config-driven**.

---

## ✨ Key Features

### 🛠️ Core Architecture

- **Custom Binary Protocol**
  - Length-prefixed framing (`type + payload_length + payload`)
  - Safe decoding over TCP streams
  - Designed for large file transfers

- **UDP Peer Discovery (LAN)**
  - Automatic peer discovery using UDP broadcast
  - No manual IP or port entry required
  - Dynamic peer table with liveness tracking

- **Config-Driven System**
  - Runtime behavior controlled via `config.ini`
  - Ports, logging, chunk size configurable without recompiling

- **Multi-Threaded Runtime**
  - UDP discovery thread
  - TCP listener thread
  - Protocol handling thread
  - File transfer execution thread

- **Robust Logging**
  - Thread-safe logger
  - Levels: INFO, DEBUG, ERROR
  - Console output + persistent `p2p.log`

---

## 🌐 Networking & Protocol Layer

- **Handshake System**
  - `HELLO` / `HELLO_ACK`
  - Peer ID exchange
  - Protocol version validation
  - Capability negotiation

- **Session Management**
  - Active TCP session tracking
  - Graceful disconnect handling
  - Timeout and liveness detection

- **Message Types**
  - `HELLO`, `HELLO_ACK`
  - `FILE_OFFER`, `FILE_ACCEPT`
  - `CHUNK_DATA`, `CHUNK_ACK`
  - `FILE_RESUME_REQUEST`
  - `PING`, `ERROR`

---

## 🔐 Security & Data Integrity

- **Encrypted Communication**
  - AES-based encryption enabled after handshake
  - Encrypted payload transmission

- **Chunk-Level Integrity**
  - Files split into fixed-size chunks (1–4 MB)
  - SHA-256 hash per chunk
  - Corrupt chunks are detected and retransmitted

---

## 📦 File Transfer System

- **Chunk-Based Transfer Engine**
  - Deterministic, reliable delivery
  - Sequential chunk scheduling
  - Per-chunk acknowledgment

- **Resume Support**
  - Transfer state persisted on disk
  - Resume after crash, kill, or network loss
  - Only missing chunks are re-requested

- **Failure Recovery**
  - Timeout-based retries
  - Hash mismatch handling
  - Safe cancellation and cleanup

---

## 🧠 Protocol Format

| Field | Size | Description |
|------|------|------------|
| Type | 2 Bytes | Message identifier |
| Length | 4 Bytes | Payload size |
| Payload | Variable | Encrypted data or control message |

---

## ⚙️ Build & Run

### 🔧 Prerequisites

- C++17 compatible compiler (MSVC / MinGW / Clang)
- CMake ≥ 3.10
- Windows (Winsock2)

---

## 🚀 How to Use

### Option 1: Run the Portable Version (Quickest)
1. **Download:** Get the `p2p_fileSharing_v1.0.zip`.
2. **Extract:** Unzip the folder (e.g., to your Desktop).
3. **Run:** Double-click `p2p.exe`. 
   > **Note:** The `libcrypto-3-x64.dll`, `libssl-3-x64.dll`, and `config.ini` **must** stay in the same folder as the `.exe`.

---

### Option 2: Build from Source(For Developers)

### 🛠️ Build Instructions

```bash
git clone https://github.com/raj-adi00/File-Sharing.git
cd File-Sharing
cd p2p

cmake -S . -B build
cmake --build build
````

---

### ▶️ Running the Application

#### Single Device (Two Terminals)

Use two different config files with different TCP ports.

**Terminal 1**

```bash
build/p2p.exe config.ini
```

**Terminal 2**

```bash
build/p2p.exe config1.ini
```

Peers will automatically discover each other, complete handshake, and allow file transfer.

---

#### Two Different Devices (LAN / Hotspot)

1. Connect both devices to the same Wi-Fi or mobile hotspot
2. Run on both devices:

```bash
build/p2p.exe
```

Peers are discovered automatically via UDP broadcast and can transfer files directly.

---

## 📁 Project Structure

```
core/        → Logger, ConfigManager
net/         → TCP & UDP networking
protocol/    → Message framing & handshake
crypto/      → Encryption engine
storage/     → File & chunk management
transfer/    → Transfer & resume logic
ui/          → CLI interface & metrics
```

---

## 🧪 Reliability Validation

* Tested with 10 MB and 500 MB files
* Forced process termination mid-transfer
* Network disconnect and reconnect
* Resume and integrity verification

Results:

* No data corruption
* Successful resume
* Stable execution

---

## 🎯 Why This Project Matters

This project demonstrates:

* Low-level networking (TCP + UDP)
* Custom binary protocol design
* Concurrent system architecture
* Fault tolerance and recovery
* Real-world file transfer engineering

Well-suited for **C++ backend, systems, and networking interviews**.

---

## 👤 Author

**Aditya Raj**
C++ | Systems Programming | Networking | Backend Engineering

---

⭐ If you find this project useful, consider starring the repository!

```
