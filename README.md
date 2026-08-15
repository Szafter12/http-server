# Custom HTTP Server

A Lightweight HTTP/1.1 Web Server Built from Scratch in C.

### What the project is
A low-level HTTP web server written entirely from scratch in C using standard POSIX APIs. Currently in active development, the server operates synchronously to serve static files (HTML, CSS, assets) over TCP/IP, with planned support for multi-threading and concurrent connection handling.

### Aim
The primary aim of this project is to gain a deep, foundational understanding of network programming and low-level systems engineering in C. It serves as a hands-on exploration of the raw socket lifecycle, manual HTTP protocol parsing, memory management, and POSIX system calls without relying on external libraries or frameworks.

### Key Features
* **Raw POSIX Socket Implementation:** Manages the full socket lifecycle (`socket`, `bind`, `listen`, `accept`, `close`).
* **Robust I/O (RIO) Handling:** Implements robust I/O helper functions to reliably read and write full payloads across network sockets without data loss.
* **HTTP/1.1 Request Parsing:** Basic parser to extract HTTP methods (GET), request URIs, and headers from incoming TCP byte streams.
* **Static File Serving:** Reads and serves local files with appropriate HTTP status codes (e.g., `200 OK`, `404 Not Found`).
* **Automated Build Pipeline:** Configured via a `Makefile` for quick compilation, testing, and cleanup.
* **Work in Progress (Roadmap):** Multi-threading support (POSIX threads / thread pooling) for concurrent request handling and dynamic routing.

### Challenges
* **Short Counts & Incomplete I/O:** Network sockets frequently encounter short counts where `read` or `write` system calls transfer fewer bytes than requested due to OS buffer constraints or signal interruptions.
    * **Solution:** Implemented Robust I/O (RIO) functions that wrap system calls in loops, ensuring the server handles short counts gracefully and transfers the full payload every time.
* **Raw Stream Parsing & Buffer Management:** Handling TCP streams where HTTP requests can arrive in fragmented chunks, avoiding buffer overflows.
    * **Solution:** Implemented controlled buffer reading and string parsing routines with strict boundary checks to extract request paths reliably.

### Setup

1. **Clone the repository:**
```bash
git clone [https://github.com/Szafter12/http-server.git](https://github.com/Szafter12/http-server.git)
cd http-server
```
2. Build the server:
```bash
make debug
```
3. Run the executable:
```bash
./bin/http_server
```
4. Run Help command to see all available commands
```bash
make help
```

### Technical concepts used
- POSIX Sockets API: Low-level TCP/IP networking and communication.
- Robust I/O (RIO): Managing buffered and unbuffered network reads/writes to eliminate short counts.
- C Systems Programming: Pointer arithmetic, manual memory management (malloc/free), and structured error handling.
- HTTP/1.1 Protocol: Understanding headers, request lines, status codes, and MIME types.
- File I/O: Reading files into buffers and streaming them over network sockets.
- GNU Make: Build automation and compilation management.
