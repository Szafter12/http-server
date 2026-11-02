# Custom HTTP Server

A High-Performance, Multi-Threaded HTTP/1.1 Web Server Built from Scratch in C.

### What the project is
A low-level HTTP/1.1 web server written entirely from scratch in C using standard POSIX APIs. The server leverages a multi-threaded architecture (POSIX Threads) to serve static files (HTML, CSS, assets) and dynamic content (via PHP-CGI) concurrently over TCP/IP, featuring robust I/O streams, defensive path sanitation, and automated regression testing.

### Aim
The primary aim of this project is to gain a deep, foundational understanding of network programming, concurrency, and low-level systems engineering in C. It serves as a hands-on exploration of the raw socket lifecycle, thread safety, manual HTTP protocol parsing, memory management, and POSIX system calls without relying on third-party libraries or web frameworks.

### Key Features
- **Raw POSIX Socket Implementation:** Manages the full socket lifecycle (`socket`, `bind`, `listen`, `accept`, `close`).
- **Concurrent Connection Handling:** Employs POSIX Threads (`pthreads`) in a thread-per-connection model. Each worker thread runs detached (`pthread_detach`) to ensure prompt, automatic OS resource cleanup upon connection termination.
- **Dynamic Content Generation (CGI):** Supports execution of server-side scripts (PHP) by utilizing POSIX process control (`fork`, `execve`, `waitpid`) and pipe redirection (`dup2`) to stream dynamic output directly to the client.
- **Robust I/O (RIO) Layer:** Thread-safe buffered and unbuffered I/O helper routines that eliminate short counts and gracefully resume on interrupt signals (`EINTR`).
- **Security & Path Sanitation:** Built-in defenses against Directory Traversal attacks (detecting and blocking `..` escapes with `403 Forbidden`).
- **Signal Resilience (`SIGPIPE` Mitigation):** Explicitly handles broken pipe conditions (`SIGPIPE`), preventing premature process termination when clients disconnect abruptly.
- **HTTP/1.1 Parsing & Query Handling:** Parses request lines and headers, correctly routing file paths while cleanly isolating and handling query strings (`?id=...`).
- **Automated Test Suite:** Shell-driven testing pipeline covering protocol compliance, concurrency stress tests, and connection teardown edges.
- **Configurable Build Automation:** Complete `Makefile` supporting optimized release binaries, debugging symbols, AddressSanitizer (`ASan`), and test automation.

---

### Challenges & Engineering Solutions

- **CGI Process Isolation & I/O Redirection:**
  - *Problem:* Executing external scripts like PHP requires creating a separate process, and the standard output of that script must be routed back to the TCP socket without the script's explicit knowledge of the network.
  - *Solution:* Utilized the classic UNIX `fork()` and `execve()` pattern combined with `dup2()` to safely duplicate the socket file descriptor onto `STDOUT_FILENO` before executing the CGI binary, seamlessly bridging the script's output with the network stream.

- **Race Conditions on Socket Descriptors:**
  - *Problem:* Passing a pointer to a stack-allocated file descriptor (`&connfd`) into `pthread_create` introduces a critical race condition where subsequent `accept()` calls overwrite the descriptor before the worker thread reads it.
  - *Solution:* Allocated a dedicated heap memory segment (`malloc`) for each accepted descriptor, transferring ownership directly to the worker thread which reads the value and immediately frees the pointer.

- **Client Disconnection & `SIGPIPE` Crashing:**
  - *Problem:* When a client closes a connection mid-stream (e.g., closing browser tabs), subsequent `write` calls trigger a kernel-level `SIGPIPE` signal, immediately killing the entire server process.
  - *Solution:* Ignored `SIGPIPE` at the process level via `signal(SIGPIPE, SIG_IGN)` and enforced per-thread error checking on all socket write routines to catch `EPIPE` and cleanly close client descriptors without crashing.

- **Short Counts & Incomplete I/O:**
  - *Problem:* Raw `read` and `write` calls frequently encounter partial transfers due to OS network buffer constraints or signal interrupts.
  - *Solution:* Implemented Robust I/O (RIO) wrapper functions that loop until the full requested byte count is transferred or a terminal error/EOF occurs.

- **Thread-Local Buffering:**
  - *Problem:* Sharing global state or static buffers across concurrent threads leads to data corruption during HTTP header parsing.
  - *Solution:* Bound all `rio_t` state structures and request buffers strictly to the thread stack, ensuring full re-entrancy and thread isolation.

---

### Quickstart & Build Targets

The project utilizes a standard Makefile for compilation and runtime lifecycle management.

### 1. Available Make Commands

Run `make help` to display all supported targets:

```bash
make help
```

### 2. Running the Server

Build and start the server using:

```bash
# Start on default port (8080)
make run

# Or start manually specifying a custom port:
./bin/http_server 9000
```

### 3. Testing Dynamic Content (PHP-CGI)

The server is capable of serving dynamic PHP content. A test file is located in the `www/php/index.php` directory. Since the server routes requests relative to the document root (`www/`), you need to specify the path in the URL.

1. Ensure the `php-cgi` binary is installed on your system (e.g., `sudo apt install php-cgi`).
2. Start the server (`make run`).
3. Open your browser or use `curl` to access the file, appending a query string to test variable parsing:

```bash
curl "http://localhost:8080/www/php/index.php?name=Test&id=42"
```

### 4. Automated Testing

Run the full automated test suite with a single command:

```bash
make test
```

The test pipeline executes three distinct test suites:

- **Functional & Compliance (test_suite.sh):**
   - Validates standard GET requests (200 OK).
   - Verifies handling of root requests (/ mapping to /index.html).
   - Ensures 404 Not Found for missing resources.
   - Verifies query strings (/file.html?id=1) are parsed without altering static file lookups.
   - Tests security against Directory Traversal (/../../etc/passwd returning 403 Forbidden).

- **Concurrency Verification (test_concurrency.sh):**
   - Dispatches 50+ simultaneous asynchronous background requests to verify that worker threads handle workloads concurrently without blocking the main listener.

- **SIGPIPE & Disconnect Resilience (test_sigpipe.sh):**
   - Opens a TCP connection via Netcat (nc), sends a partial HTTP header, and abruptly terminates the connection to ensure the server handles the broken pipe gracefully and remains alive.

---

### Technical Concepts Used

- **POSIX Sockets API:** Low-level TCP/IP networking (socket, bind, listen, accept).
- **Process Control & IPC:** Process cloning (`fork`), execution (`execve`), and I/O redirection (`dup2`) to build a Common Gateway Interface (CGI).
- **POSIX Threads (pthreads):** Concurrent thread creation, thread detachment, and heap-allocated descriptor passing to eliminate race conditions.
- **Robust I/O (RIO):** Buffered and unbuffered streaming routines immune to network short counts.
- **Signals & Process Safety:** Signal masking and explicit handling of SIGPIPE and EPIPE.
- **Memory Safety & Diagnostics:** Verification through GCC AddressSanitizer (-fsanitize=address) and Valgrind.
- **HTTP/1.1 Protocol:** Manual byte-stream parsing of request lines, headers, and status codes.
- **GNU Make & Bash Automation:** Complete CI-ready build orchestration and integration test scripting.
