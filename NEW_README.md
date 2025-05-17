# 📡 Minitalk

<div align="center">
  <img src="https://img.shields.io/badge/Language-C-blue" alt="Language">
  <img src="https://img.shields.io/badge/Project-42-brightgreen" alt="Project">
  <img src="https://img.shields.io/badge/Status-In%20Progress-yellow" alt="Status">
</div>

## 📚 Table of Contents
- [Introduction](#introduction)
- [Project Requirements](#project-requirements)
- [How It Works](#how-it-works)
- [Installation and Usage](#installation-and-usage)
- [Implementation Details](#implementation-details)
- [Challenges and Solutions](#challenges-and-solutions)
- [Bonus Features](#bonus-features)
- [Testing](#testing)
- [Project Structure](#project-structure)
- [Lessons Learned](#lessons-learned)
- [Resources](#resources)

## 🌟 Introduction

Minitalk is a communication program that demonstrates the use of UNIX signals for inter-process communication (IPC). This project implements a client-server architecture where:
- The server displays its Process ID (PID) and waits for signals
- The client sends a string to the server using ONLY SIGUSR1 and SIGUSR2 signals
- The server reconstructs and displays the received message

What makes this project fascinating is the challenge of transmitting complex data using only two distinct signals - essentially creating a binary communication protocol from scratch!

## 🎯 Project Requirements

### Mandatory Part
- Create a communication program with a client and server
- The server must start first and display its PID after launch
- The client takes two parameters:
  - The server PID
  - The string to send to the server
- The client must send the string to the server using ONLY SIGUSR1 and SIGUSR2 signals
- The server must display the received string
- The server should be able to receive strings from multiple clients in succession

### Bonus Part
- Add server acknowledgment of received signals
- Support Unicode characters

## 🔄 How It Works

### Signal-Based Communication

UNIX signals are software interrupts that provide a way to handle asynchronous events. In this project, we'll use:

- **SIGUSR1** (User-defined signal 1)
- **SIGUSR2** (User-defined signal 2)

Since we're limited to just two signals, we need to devise a binary encoding/decoding scheme:

```text
SIGUSR1 → Bit 0
SIGUSR2 → Bit 1
```

### Binary Data Transmission Process

The entire communication process works as follows:

1. The server starts and displays its PID
2. The client takes this PID and the string to send as arguments
3. For each character in the string:
   - Convert the character to its binary representation (8 bits)
   - Send each bit as a signal: SIGUSR1 for bit 0, SIGUSR2 for bit 1
4. The server receives the signals, reconstructs each character, and displays it

```
+---------+                           +---------+
|         |  SIGUSR1 (Bit 0)          |         |
| CLIENT  | ------------------------> | SERVER  |
|         |  SIGUSR2 (Bit 1)          |         |
+---------+                           +---------+
    |                                      |
Converts                               Reconstructs
characters                            characters from
to bits                                 received bits
    |                                      |
    v                                      v
 "Hello"                                "Hello"
```

### Communication Protocol

1. **Server Initialization**:
   - Server starts and prints its PID
   - Sets up signal handlers for SIGUSR1 and SIGUSR2
   - Waits for incoming signals

2. **Client Operation**:
   - Takes server PID and string as parameters
   - For each character in the string:
     - Converts character to binary (8 bits)
     - Sends each bit as SIGUSR1 (0) or SIGUSR2 (1)
     - Waits for acknowledgment (optional for bonus)

3. **Message Termination**:
   - After sending the full string, client can send a special termination sequence
   - For example, sending a null character (8 zeros)

### Example: Sending Character 'A'

The ASCII value of 'A' is 65, which in binary is 01000001.

| Bit Position | Binary Value | Signal Sent |
|--------------|--------------|------------|
| 7 (MSB)      | 0            | SIGUSR1    |
| 6            | 1            | SIGUSR2    |
| 5            | 0            | SIGUSR1    |
| 4            | 0            | SIGUSR1    |
| 3            | 0            | SIGUSR1    |
| 2            | 0            | SIGUSR1    |
| 1            | 0            | SIGUSR1    |
| 0 (LSB)      | 1            | SIGUSR2    |

## 💻 Installation and Usage

### Prerequisites
- GCC compiler
- Make
- UNIX-based operating system (Linux, macOS)

### Compilation
```bash
# Clone the repository
git clone https://github.com/yourusername/minitalk.git
cd minitalk

# Compile the project
make

# Compile with bonus features
make bonus  # If implementing bonus features
```

### Running the Program

1. Start the server in one terminal:
```bash
./server
```

2. Note the PID displayed by the server

3. Run the client in another terminal with the server's PID and your message:
```bash
./client <server_pid> "Hello, Minitalk!"
```

4. Watch as the message appears in the server's terminal!

## 🛠️ Implementation Details

### Server Implementation

```c
// Core structure for the server
void handle_signal(int signum);
void setup_signals();
void display_pid();

int main() {
    display_pid();
    setup_signals();
    
    while (1) {
        // Keep the server running and waiting for signals
        pause();
    }
    return 0;
}
```

### Client Implementation

```c
// Core structure for the client
void send_char(int pid, char c);
void send_string(int pid, char *str);

int main(int argc, char **argv) {
    if (argc != 3) {
        // Error handling
        return 1;
    }
    
    int server_pid = atoi(argv[1]);
    char *message = argv[2];
    
    send_string(server_pid, message);
    return 0;
}
```

### Signal Handling Logic

The server will need to:

1. Receive bits (SIGUSR1/SIGUSR2)
2. Buffer the bits until a complete character is formed (8 bits)
3. Convert the buffered bits to a character and display it
4. Clear the buffer for the next character

A sample buffer implementation might look like:

```c
// Global variables
static int g_bit_count = 0;
static unsigned char g_char_buffer = 0;

void handle_signal(int signum) {
    // Shift buffer and add new bit
    g_char_buffer <<= 1;
    if (signum == SIGUSR2)
        g_char_buffer |= 1;  // Set bit to 1 for SIGUSR2
    
    g_bit_count++;
    
    // When we have a complete character (8 bits)
    if (g_bit_count == 8) {
        // Display the character
        write(1, &g_char_buffer, 1);
        
        // Reset buffer and counter for next character
        g_char_buffer = 0;
        g_bit_count = 0;
    }
}
```

The client will need to:

1. Convert each character to binary
2. Send each bit as a signal
3. Use proper timing or acknowledgment to ensure reliability

## ⚠️ Challenges and Solutions

### Signal Reliability Issues

**Challenge**: UNIX signals can be lost, especially when sent too quickly.

**Solution**: 
- Implement a small delay between signal transmissions
- Use `usleep()` between sending signals to avoid overwhelming the server
- Typical delay: 100-150 microseconds

```c
void send_bit(int pid, int bit) {
    if (kill(pid, bit ? SIGUSR2 : SIGUSR1) < 0) {
        perror("Signal sending failed");
        exit(EXIT_FAILURE);
    }
    usleep(100);  // Small delay to ensure signal reception
}
```

### Race Conditions

**Challenge**: Signal handlers can create race conditions with shared data.

**Solution**:
- Use `volatile` keyword for variables accessed in signal handlers
- Keep signal handlers simple and atomic
- Block signals during critical sections

```c
volatile sig_atomic_t g_bit_count = 0;
volatile sig_atomic_t g_char_buffer = 0;
```

### Signal Blocking in Critical Sections

**Challenge**: Avoiding interruption of critical operations.

**Solution**: Use signal masks to temporarily block signals.

```c
void critical_section() {
    sigset_t block_mask, old_mask;
    
    // Create mask and block signals
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGUSR1);
    sigaddset(&block_mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &block_mask, &old_mask);
    
    // Perform critical operations safely
    
    // Restore original signal mask
    sigprocmask(SIG_SETMASK, &old_mask, NULL);
}
```

## 🌟 Bonus Features

### 1. Acknowledgment Protocol

For robust communication, the server can acknowledge each received bit:

```c
// Server side
void handle_signal(int signum, siginfo_t *info, void *context) {
    (void)context;
    
    // Process the bit...
    
    // Send acknowledgment back to client
    kill(info->si_pid, SIGUSR1);
}
```

```c
// Client side
void wait_for_ack() {
    volatile sig_atomic_t ack_received = 0;
    
    // Setup signal handler for acknowledgment
    signal(SIGUSR1, handle_ack);
    
    // Wait for acknowledgment with timeout
    alarm(1);  // Set timeout of 1 second
    while (!ack_received) {
        pause();
    }
    alarm(0);  // Cancel timeout
}

void handle_ack(int signum) {
    (void)signum;
    ack_received = 1;
}
```

### 2. Unicode Support

Supporting UTF-8 encoding requires handling multi-byte characters:

```c
void send_utf8_char(int pid, wchar_t wc) {
    char utf8_buffer[5] = {0};
    int len;
    
    // Convert wide character to UTF-8
    len = wctomb(utf8_buffer, wc);
    
    // Send each UTF-8 byte
    for (int i = 0; i < len; i++) {
        send_char(pid, utf8_buffer[i]);
    }
}
```

## 🧪 Testing Strategy

### 1. Basic Functionality Testing

```bash
# Test with simple message
./client `pidof server` "Hello, World!"

# Test with empty message
./client `pidof server` ""

# Test with special characters
./client `pidof server` "!@#$%^&*()_+"
```

### 2. Unicode Support Testing (bonus)

```bash
# Test with various languages
./client `pidof server` "こんにちは"  # Japanese
./client `pidof server` "Привет"     # Russian
./client `pidof server` "你好"       # Chinese
```

### 3. Stress Testing

```bash
# Test with very long message
./client `pidof server` "$(cat lorem_ipsum.txt)"

# Test with rapid client connections
for i in {1..10}; do
  ./client `pidof server` "Message $i" &
done
```

### 4. Error Handling Tests

```bash
# Test with invalid PID
./client 99999 "This should fail"

# Kill server during transmission
# (Start transmission, then quickly kill server)
```

## 📁 Project Structure

Here's a comprehensive view of the project files with their contents:

```plaintext
Minitalk/
│
├── Makefile              # Build configuration
│   ├── all: Build both client and server
│   ├── clean: Remove object files
│   ├── fclean: Remove object files and executables
│   └── re: Rebuild from scratch
│
├── include/
│   └── minitalk.h        # Shared header file
│       ├── Includes: <signal.h>, <unistd.h>, etc.
│       ├── Function prototypes
│       └── Common definitions/constants
│
├── src/
│   │
│   ├── client.c          # Client implementation
│   │   ├── send_bit(): Sends a single bit via signals
│   │   ├── send_char(): Converts char to bits and sends
│   │   ├── send_string(): Processes entire string
│   │   └── main(): Entry point, handles arguments
│   │
│   └── server.c          # Server implementation
│       ├── handle_signal(): Signal handler that buffers bits
│       ├── setup_signals(): Sets up signal handlers
│       ├── display_pid(): Shows server PID
│       └── main(): Entry point, infinite signal waiting loop
│
└── bonus/                # Optional bonus implementations
    ├── client_bonus.c    # Client with acknowledgment support
    │   ├── handle_ack(): Handles server acknowledgments
    │   ├── send_bit(): With timeout/retry mechanism
    │   └── unicode_support(): For handling multi-byte chars
    │
    └── server_bonus.c    # Server with enhanced features
        ├── send_ack(): Sends acknowledgment signals
        ├── unicode_buffer(): For multi-byte character support
        └── signal_handler(): With acknowledgment feature
```

### Development Workflow

1. **Setup Phase**:
   - Create the directory structure
   - Create header file with necessary includes/definitions
   - Implement Makefile

2. **Implementation Phase**:
   - Implement server.c (signal handling, bit buffering)
   - Implement client.c (char-to-bit conversion, signal sending)
   - Test and debug communication

3. **Bonus Phase** (optional):
   - Add acknowledgment mechanism
   - Implement Unicode support
   - Enhance error handling

4. **Testing Phase**:
   - Test with various inputs
   - Verify corner cases
   - Benchmark performance

## 📝 Lessons Learned

### Signal Handling Insights

- **Signal Limitations**: UNIX signals are not designed for high-throughput data transfer, but with proper timing and error handling, they can be reliable.

- **Race Conditions**: Signal handlers interrupt normal program flow, making race conditions a constant concern when handling shared data.

- **Signal Queuing**: Most UNIX systems don't queue identical signals, meaning rapid signal transmission can result in lost signals.

### Binary Protocols

- **Encoding Scheme**: Building a binary protocol requires careful consideration of bit ordering, character encoding, and transmission reliability.

- **Data Framing**: Without dedicated framing signals, creative solutions (like sending null characters) are needed to indicate message boundaries.

### Performance Considerations

- **Timing is Critical**: Finding the optimal delay between signals is crucial—too short and signals get lost, too long and performance suffers.

- **Buffer Management**: Efficient buffer management in signal handlers is essential for reliable character reconstruction.

## 📚 Resources

### Signal Programming

- [The Linux Programming Interface](https://man7.org/tlpi/) - Comprehensive guide to Linux/UNIX system programming
- [Advanced Programming in the UNIX Environment](http://www.apuebook.com/) - Classic reference for UNIX programming
- [Signal Concepts](https://man7.org/linux/man-pages/man7/signal.7.html) - Linux manual page for signals

### Binary Protocols

- [Binary Protocol Basics](https://blog.mbedded.ninja/programming/serialization-formats/binary-serialization-formats/binary-serialization-formats-overview/) - Introduction to binary protocols
- [Character Encoding](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/) - Essential knowledge about character encoding

### Debugging Tools

- [strace](https://man7.org/linux/man-pages/man1/strace.1.html) - Useful for tracking signals
- [GDB](https://www.gnu.org/software/gdb/) - For debugging signal-related issues

---

*This project is part of the 42 School curriculum.*
