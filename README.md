# 📡 Minitalk - Complete Tutorial & Implementation Guide

![Language](https://img.shields.io/badge/Language-C-blue)
![Status](https://img.shields.io/badge/Status-Completed-success)
![42School](https://img.shields.io/badge/School-42-black)

## 🌟 Introduction

Minitalk is a sophisticated inter-process communication (IPC) system that demonstrates how two processes can communicate using only UNIX signals. This project showcases binary communication, signal handling, and bit manipulation techniques in C.

```ascii
┌─────────────┐                    ┌─────────────┐
│   CLIENT    │ ──── Signals ────► │   SERVER    │
│             │                    │             │
│ "Hello" ────┼──► 0101000001001000 │ ──► "Hello" │
│             │    (binary data)   │             │
└─────────────┘                    └─────────────┘
```

The client sends text messages to the server bit by bit using only two signals:
- **`SIGUSR1`** represents bit **0**
- **`SIGUSR2`** represents bit **1**

## 🏗️ Project Architecture

```
Minitalk/
├── minitalk.h              # Main header with function declarations
├── minitalk_comments.h     # Documented function prototypes
├── Makefile               # Build system
├── srcs/
│   ├── server.c           # Server implementation
│   ├── client.c           # Client implementation
│   └── ft_utils.c         # Utility functions (I/O, conversions)
├── README.md              # This comprehensive guide
└── .gitignore             # Git ignore patterns
```

## 🛠️ Core Concepts

### 1. Binary Signal Communication

Every character is transmitted as 8 individual bits, each bit becoming a signal:

```ascii
ASCII 'A' (65) = 01000001 in binary
  ┌───┬───┬───┬───┬───┬───┬───┬───┐
  │ 0 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │
  └─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┘
    │   │   │   │   │   │   │   │
    ▼   ▼   ▼   ▼   ▼   ▼   ▼   ▼
  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐
  │SIGU1│ │SIGU2│ │SIGU1│ │SIGU2│
  └─────┘ └─────┘ └─────┘ └─────┘

Signal sequence: SIGUSR1→SIGUSR2→SIGUSR1→SIGUSR1→SIGUSR1→SIGUSR1→SIGUSR1→SIGUSR2
```

### 2. Character Transmission Process

```ascii
Process Flow:
1. Character → 8 bits
2. Each bit → Signal (SIGUSR1 for 0, SIGUSR2 for 1)
3. Server reconstructs bits → Character
4. Buffer management → Complete message
```

## 📁 Implementation Deep Dive

### 🖥️ Client Side ([`srcs/client.c`](srcs/client.c))

#### Function: [`s_bit`](srcs/client.c) - Atomic Bit Transmission

```c
void s_bit(int pid, int bit)
{
    if (bit == 0)
        kill(pid, SIGUSR1);  // Send SIGUSR1 for bit 0
    else
        kill(pid, SIGUSR2);  // Send SIGUSR2 for bit 1
    usleep(BIT_DELAY);       // Wait to prevent signal collision
}
```

**Visual Flow:**
```ascii
    Input bit
        │
        ▼
    ┌───────┐     ┌─ bit==0 ─┐     ┌─────────────┐
    │ bit=0 │────►│          │────►│  SIGUSR1    │───► To Server
    │   or  │     │ Decision │     │     or      │
    │ bit=1 │────►│          │────►│  SIGUSR2    │───► To Server
    └───────┘     └─ bit==1 ─┘     └─────────────┘
                                           │
                                           ▼
                                   ┌─────────────┐
                                   │ usleep(100) │ (Prevent collision)
                                   └─────────────┘
```

#### Function: [`s_char`](srcs/client.c) - Character Decomposition

```c
void s_char(int pid, char c)
{
    int i = 7;                    // Start from MSB
    while (i >= 0)
    {
        s_bit(pid, (c >> i) & 1); // Extract and send each bit
        i--;
    }
}
```

**Bit Extraction Magic: `(c >> i) & 1`**

```ascii
Character 'A' = 01000001 in binary
                ↑↑↑↑↑↑↑↑
Bit positions:   76543210

Step-by-step extraction:
i=7: Extract bit 7 (MSB)
   01000001 >> 7 = 00000000 (shifted 7 positions right)
   00000000 & 1 = 0         (AND with 1 extracts last bit)
   Result: 0 → Send SIGUSR1

i=6: Extract bit 6
   01000001 >> 6 = 00000001 (shifted 6 positions right)
   00000001 & 1 = 1         (AND with 1 extracts last bit)
   Result: 1 → Send SIGUSR2

i=0: Extract bit 0 (LSB)
   01000001 >> 0 = 01000001 (no shift)
   01000001 & 1 = 1         (AND with 1 extracts last bit)
   Result: 1 → Send SIGUSR2

Signal transmission table for 'A':
┌────────────┬────────────┬──────────────┬───────────────┐
│  Position  │  Bit Value │  Signal Sent │  Operation    │
├────────────┼────────────┼──────────────┼───────────────┤
│   Bit 7    │      0     │    SIGUSR1   │  (c>>7) & 1   │
│   Bit 6    │      1     │    SIGUSR2   │  (c>>6) & 1   │
│   Bit 5    │      0     │    SIGUSR1   │  (c>>5) & 1   │
│   Bit 4    │      0     │    SIGUSR1   │  (c>>4) & 1   │
│   Bit 3    │      0     │    SIGUSR1   │  (c>>3) & 1   │
│   Bit 2    │      0     │    SIGUSR1   │  (c>>2) & 1   │
│   Bit 1    │      0     │    SIGUSR1   │  (c>>1) & 1   │
│   Bit 0    │      1     │    SIGUSR2   │  (c>>0) & 1   │
└────────────┴────────────┴──────────────┴───────────────┘
```

#### Client Main Function - Message Processing

```ascii
Client execution flow:
┌─────────────┐
│ Start       │
└─────────────┘
       │
       ▼
┌─────────────┐
│ Parse args  │ ──── Check argc == 3
└─────────────┘
       │
       ▼
┌─────────────┐
│ Get PID     │ ──── ft_atoi(argv[1])
└─────────────┘
       │
       ▼
┌─────────────┐
│ Get message │ ──── argv[2]
└─────────────┘
       │
       ▼
┌─────────────┐      ┌──────────────┐
│ For each    │ ────►│ s_char(pid,  │
│ character   │      │ str[i])      │
└─────────────┘      └──────────────┘
       │                     │
       │◄────────────────────┘
       ▼
┌─────────────┐
│ Send '\0'   │ ──── Signal end of message
└─────────────┘
```

### 🖲️ Server Side ([`srcs/server.c`](srcs/server.c))

#### Function: [`x_signal`](srcs/server.c) - The Signal Processing Engine

```c
void x_signal(int signum)
{
    static int             bit_count = 0;  // Count received bits
    static unsigned char   c = 0;          // Current character being built
    static char           buf[BUF_SIZE];   // Message buffer
    static int            idx = 0;         // Buffer index

    c <<= 1;                              // Shift left: make room for new bit
    if (signum == SIGUSR2)                // If signal is SIGUSR2 (bit 1)
        c |= 1;                           // Set rightmost bit to 1
    bit_count++;                          // Increment bit counter
    
    if (bit_count == 8)                   // Full character received?
    {
        buf[idx++] = c;                   // Store character in buffer
        if (c == '\0' || idx >= BUF_SIZE - 1)  // End of message?
        {
            buf[idx - 1] = '\0';          // Null-terminate
            ft_putstr(buf);               // Print message
            ft_putchar('\n');
            idx = 0;                      // Reset buffer
        }
        bit_count = 0;                    // Reset bit counter
        c = 0;                            // Reset character
    }
}
```

**Character Reconstruction Process:**

```ascii
Rebuilding character 'A' (01000001):

Initial state: c = 00000000, bit_count = 0

Signal 1: SIGUSR1 (bit 0)
   c <<= 1        →  c = 00000000  (shift left)
   signum != SIGUSR2, so no OR operation
   bit_count = 1

Signal 2: SIGUSR2 (bit 1)
   c <<= 1        →  c = 00000000  (shift left)
   c |= 1         →  c = 00000001  (set rightmost bit)
   bit_count = 2

Signal 3: SIGUSR1 (bit 0)
   c <<= 1        →  c = 00000010  (shift left)
   signum != SIGUSR2, so no OR operation
   bit_count = 3

... (continue for 8 signals) ...

Signal 8: SIGUSR2 (bit 1)
   c <<= 1        →  c = 01000000  (shift left)
   c |= 1         →  c = 01000001  (set rightmost bit)
   bit_count = 8  →  Character complete! c = 'A'
```

**Buffer Management System:**

```ascii
Message: "Hi"

After receiving 'H' (01001000):
┌─┬─┬─┬─┬─┬─┬─┬─┬─┬───┐
│H│ │ │ │ │ │ │ │ │...│  buf
└─┴─┴─┴─┴─┴─┴─┴─┴─┴───┘
  ↑
 idx=1

After receiving 'i' (01101001):
┌─┬─┬─┬─┬─┬─┬─┬─┬─┬───┐
│H│i│ │ │ │ │ │ │ │...│  buf
└─┴─┴─┴─┴─┴─┴─┴─┴─┴───┘
    ↑
   idx=2

After receiving '\0':
┌─┬─┬─┬─┬─┬─┬─┬─┬─┬───┐
│H│i│\0│ │ │ │ │ │ │...│  buf → Print "Hi" and reset
└─┴─┴─┴─┴─┴─┴─┴─┴─┴───┘
```

#### Server Lifecycle

```ascii
Server initialization and operation:
┌────────────────┐
│  Start server  │
└────────────────┘
        │
        ▼
┌────────────────┐
│  Get and show  │ ──── print_pid() from ft_utils.c
│  server PID    │
└────────────────┘
        │
        ▼
┌────────────────────────────────────┐
│  Register signal handlers          │
│  SIGUSR1 and SIGUSR2 → x_signal()  │
└────────────────────────────────────┘
        │
        ▼
┌────────────────┐
│                │
│  Wait for      │◄───────┐
│  signals       │        │
│                │        │
└────────────────┘        │
        │                 │
        │ Signal received │
        ▼                 │
┌────────────────┐        │
│ Process signal │        │
│  in x_signal() │────────┘
└────────────────┘
```

### 🔧 Utility Functions ([`srcs/ft_utils.c`](srcs/ft_utils.c))

#### [`ft_atoi`](srcs/ft_utils.c) - String to Integer Conversion

```ascii
Input: "12345"
Process:
  "12345" → skip whitespace → check sign → convert digits
  │
  ├─ '1' → res = 0*10 + 1 = 1
  ├─ '2' → res = 1*10 + 2 = 12
  ├─ '3' → res = 12*10 + 3 = 123
  ├─ '4' → res = 123*10 + 4 = 1234
  └─ '5' → res = 1234*10 + 5 = 12345

Output: 12345 (for server PID parsing)
```

#### Output Functions
- [`ft_putchar`](srcs/ft_utils.c): Atomic character output
- [`ft_putstr`](srcs/ft_utils.c): String output using ft_putchar
- [`ft_putnbr`](srcs/ft_utils.c): Recursive integer output
- [`print_pid`](srcs/ft_utils.c): Server PID display with formatting

## 🔄 Complete Communication Flow

```ascii
COMPLETE MESSAGE TRANSMISSION: "Hi"

CLIENT SIDE:                           SERVER SIDE:
┌─────────────┐                       ┌─────────────┐
│ "./client   │                       │ "./server"  │
│ 1234 Hi"    │                       │             │
└─────────────┘                       │ PID: 1234   │
       │                              └─────────────┘
       │ Parse PID & message                 │
       ▼                                     │ Waiting for signals
┌─────────────┐                             │
│ Send 'H'    │ ──────────────────────────► │
│ 01001000    │                             │
└─────────────┘                             │
   │ │ │ │ │ │ │ │                         │
   │ │ │ │ │ │ │ └─ SIGUSR1 ──────────────► │ x_signal(): bit_count=1, c=00000000
   │ │ │ │ │ │ └─── SIGUSR1 ──────────────► │ x_signal(): bit_count=2, c=00000000
   │ │ │ │ │ └───── SIGUSR1 ──────────────► │ x_signal(): bit_count=3, c=00000000
   │ │ │ │ └─────── SIGUSR2 ──────────────► │ x_signal(): bit_count=4, c=00001000
   │ │ │ └───────── SIGUSR1 ──────────────► │ x_signal(): bit_count=5, c=00010000
   │ │ └─────────── SIGUSR1 ──────────────► │ x_signal(): bit_count=6, c=00100000
   │ └───────────── SIGUSR1 ──────────────► │ x_signal(): bit_count=7, c=01000000
   └─────────────── SIGUSR1 ──────────────► │ x_signal(): bit_count=8, c=01001000='H'
                                             │ Store 'H' in buffer[0]
┌─────────────┐                             │
│ Send 'i'    │ ──────────────────────────► │
│ 01101001    │                             │
└─────────────┘                             │
   8 more signals... ────────────────────► │ Reconstruct 'i', store in buffer[1]
                                             │
┌─────────────┐                             │
│ Send '\0'   │ ──────────────────────────► │
│ 00000000    │                             │
└─────────────┘                             │
   8 more signals... ────────────────────► │ Receive '\0' → Print "Hi\n"
```

## ⚙️ Build System Analysis

The [`Makefile`](Makefile) efficiently manages compilation:

```ascii
Build process:
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ srcs/*.c    │────►│ Compile     │────►│ *.o files   │
│ files       │     │ with flags  │     │             │
└─────────────┘     └─────────────┘     └─────────────┘
                                               │
                                               ▼
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ Link        │◄────│ Object      │◄────│ Group by    │
│ executables │     │ files       │     │ target      │
└─────────────┘     └─────────────┘     └─────────────┘
       │
       ▼
┌─────────────┐
│ server +    │
│ client      │
│ executables │
└─────────────┘
```

**Key Features:**
- Separate object files for client and server
- Shared utility functions ([`srcs/ft_utils.c`](srcs/ft_utils.c))
- Strict compilation flags: `-Wall -Wextra -Werror`
- Clean build targets for development

## 🚀 Installation and Usage

### 1. Compilation
```bash
make          # Build both executables
make clean    # Remove object files
make fclean   # Remove all generated files
make re       # Rebuild everything
```

### 2. Execution
```bash
# Terminal 1: Start server
./server
# Output: Server PID: 1234

# Terminal 2: Send message
./client 1234 "Hello World!"
# Server displays: Hello World!
```

### 3. Example Session
```bash
$ make
gcc -Wall -Wextra -Werror -I. -c srcs/server.c -o srcs/server.o
gcc -Wall -Wextra -Werror -I. -c srcs/ft_utils.c -o srcs/ft_utils.o
gcc -Wall -Wextra -Werror -I. -o server srcs/server.o srcs/ft_utils.o
gcc -Wall -Wextra -Werror -I. -c srcs/client.c -o srcs/client.o
gcc -Wall -Wextra -Werror -I. -o client srcs/client.o srcs/ft_utils.o

$ ./server &
Server PID: 42424

$ ./client 42424 "42 School rocks!"
42 School rocks!

$ ./client 42424 "Binary communication is amazing! 🚀"
Binary communication is amazing! 🚀
```

## 🎓 Learning Outcomes & Technical Insights

### 1. **Advanced Signal Programming**
- Understanding UNIX signals as IPC mechanism
- Signal handlers and their execution context
- Race condition prevention with timing delays
- Signal safety and limitations

### 2. **Bit Manipulation Mastery**
- Binary representation of ASCII characters
- Bitwise operations: shifting (`<<`, `>>`), masking (`&`), setting (`|`)
- Building bytes from individual bits
- Understanding endianness in bit transmission

### 3. **Memory Management with Static Variables**
- State persistence between function calls
- Static variable behavior in signal handlers
- Buffer management for message reconstruction

### 4. **Inter-Process Communication (IPC)**
- Process identification with PIDs
- Asynchronous communication patterns
- Message protocol design (start, data, termination)
- Error handling in distributed systems

### 5. **Systems Programming Concepts**
- Process lifecycle management
- System call usage (`kill`, `getpid`, `signal`)
- Signal delivery guarantees and limitations
- Real-time constraints in communication

## 🔬 Advanced Features & Optimizations

### Timing Considerations
The `BIT_DELAY` constant (100 microseconds) prevents signal loss:
```c
#define BIT_DELAY 100  // Microseconds between signals
```

### Buffer Overflow Protection
The server implements buffer boundary checking:
```c
if (c == '\0' || idx >= BUF_SIZE - 1)
{
    buf[idx - 1] = '\0';  // Ensure null termination
    // ... display and reset
}
```

### Signal Safety
The signal handler uses only async-signal-safe operations and maintains minimal state.

## 🎯 Project Significance

This project elegantly demonstrates that **complex communication can be achieved with minimal primitives**. Using just two signals (effectively binary digits), any digital information can be transmitted between processes, showcasing fundamental principles of:

- **Digital Communication**: Binary encoding of information
- **Protocol Design**: Message framing and termination
- **Systems Programming**: Low-level process interaction
- **Computer Science Theory**: Information theory in practice

The Minitalk project serves as an excellent introduction to systems programming, network protocols, and the fundamental concepts underlying all digital communication systems.

---

**Developed by anpayot for 42 Lausanne** 🎓