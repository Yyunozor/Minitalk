# 📚 Minitalk Functions Reference Guide

**Complete manual summary for all functions used in the Minitalk project**  
*Developed by anpayot for 42 Lausanne* 🎓

---

## 🔧 System Calls & Library Functions

### 1. `signal()` - Signal Handler Registration

```c
#include <signal.h>
void (*signal(int sig, void (*func)(int)))(int);
```

**Purpose:** Register a signal handler function for a specific signal type.

**Parameters:**
- `sig`: Signal number (e.g., SIGUSR1, SIGUSR2)
- `func`: Pointer to handler function or SIG_IGN/SIG_DFL

**Return:** Previous signal handler, or SIG_ERR on error

**Usage in Minitalk:**
```c
signal(SIGUSR1, my_handler);  // Register my_handler for SIGUSR1
signal(SIGUSR2, my_handler);  // Register my_handler for SIGUSR2
```

**Notes:**
- Each process can have different handlers for each signal
- Signal handlers are called asynchronously when signal arrives
- Used in server to catch incoming bit signals from client

---

### 2. `kill()` - Send Signal to Process

```c
#include <signal.h>
int kill(pid_t pid, int sig);
```

**Purpose:** Send a signal to a specific process or group of processes.

**Parameters:**
- `pid`: Process ID (target process)
- `sig`: Signal to send (SIGUSR1, SIGUSR2, etc.)

**Return:** 0 on success, -1 on error

**Usage in Minitalk:**
```c
kill(server_pid, SIGUSR1);  // Send bit 0 to server
kill(server_pid, SIGUSR2);  // Send bit 1 to server
```

**Error Conditions:**
- `ESRCH`: No such process
- `EPERM`: Permission denied
- `EINVAL`: Invalid signal

---

### 3. `getpid()` - Get Process ID

```c
#include <unistd.h>
pid_t getpid(void);
```

**Purpose:** Get the process ID of the calling process.

**Parameters:** None

**Return:** Process ID (always succeeds, never fails)

**Usage in Minitalk:**
```c
pid_t my_pid = getpid();
printf("Server PID: %d\n", my_pid);  // Server displays its PID
```

**Notes:**
- PIDs are unique system-wide
- Used by server to display its PID for client connection
- PID is required by client to send signals to server

---

### 4. `pause()` - Wait for Signal

```c
#include <unistd.h>
int pause(void);
```

**Purpose:** Suspend the calling process until a signal is received.

**Parameters:** None

**Return:** Always returns -1 (when signal handler returns)

**Usage in Minitalk:**
```c
while (1)
    pause();  // Server waits indefinitely for signals
```

**Notes:**
- Process is completely suspended until signal arrives
- More efficient than busy waiting in a loop
- Signal handler executes, then pause() returns

---

### 5. `usleep()` - Microsecond Sleep

```c
#include <unistd.h>
int usleep(useconds_t usec);
```

**Purpose:** Suspend execution for a specified number of microseconds.

**Parameters:**
- `usec`: Number of microseconds to sleep (1 second = 1,000,000 μs)

**Return:** 0 on success, -1 on error

**Usage in Minitalk:**
```c
usleep(100);     // Sleep for 100 microseconds
usleep(1000);    // Sleep for 1 millisecond (0.001 seconds)
```

**Time Conversions:**
- 1 second = 1,000,000 microseconds
- 1 millisecond = 1,000 microseconds
- Used to prevent signal collision and ensure proper timing

---

### 6. `write()` - Write to File Descriptor

```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```

**Purpose:** Write data to a file descriptor.

**Parameters:**
- `fd`: File descriptor (0=stdin, 1=stdout, 2=stderr)
- `buf`: Buffer containing data to write
- `count`: Number of bytes to write

**Return:** Number of bytes written, or -1 on error

**Usage in Minitalk:**
```c
write(1, "Hello\n", 6);    // Write to stdout
write(2, "Error\n", 6);    // Write to stderr
```

**Standard File Descriptors:**
- `0`: stdin (standard input)
- `1`: stdout (standard output)
- `2`: stderr (standard error)

---

## 🧮 String/Number Conversion Functions

### 7. `ft_atoi()` - ASCII to Integer (Custom Implementation)

```c
int ft_atoi(const char *str);
```

**Purpose:** Convert string representation of number to integer.

**Parameters:**
- `str`: Null-terminated string containing number

**Return:** Integer value

**Usage in Minitalk:**
```c
int pid = ft_atoi(argv[1]);  // Convert PID from command line argument
```

**Implementation Notes:**
- Handles leading whitespace
- Processes optional '+' or '-' sign
- Stops at first non-digit character
- Should handle integer overflow (implementation dependent)

---

### 8. `ft_putchar()` - Put Character (Custom Implementation)

```c
void ft_putchar(char c);
```

**Purpose:** Write a single character to stdout.

**Parameters:**
- `c`: Character to write

**Usage in Minitalk:**
```c
ft_putchar('A');      // Prints 'A'
ft_putchar('\n');     // Prints newline
```

**Typical Implementation:**
```c
void ft_putchar(char c)
{
    write(1, &c, 1);
}
```

---

### 9. `ft_putstr()` - Put String (Custom Implementation)

```c
void ft_putstr(char *str);
```

**Purpose:** Write a string to stdout.

**Parameters:**
- `str`: Null-terminated string to write

**Usage in Minitalk:**
```c
ft_putstr("Server PID: ");
ft_putstr("Message received\n");
```

**Typical Implementation:**
```c
void ft_putstr(char *str)
{
    while (*str)
        ft_putchar(*str++);
}
```

---

### 10. `ft_putnbr()` - Put Number (Custom Implementation)

```c
void ft_putnbr(int n);
```

**Purpose:** Write integer as string to stdout.

**Parameters:**
- `n`: Integer to write

**Usage in Minitalk:**
```c
ft_putnbr(getpid());  // Print process ID
ft_putnbr(42);        // Prints "42"
```

**Special Cases to Handle:**
- Negative numbers (print '-' first)
- INT_MIN (-2147483648) - special case due to overflow
- Zero

---

## 🎯 Signal Constants

### Signal Types for Minitalk:

| Signal | Value | Usage in Minitalk |
|--------|-------|-------------------|
| `SIGUSR1` | 10 | Represents bit 0 |
| `SIGUSR2` | 12 | Represents bit 1 |

### Signal Handler Constants:

| Constant | Purpose |
|----------|---------|
| `SIG_DFL` | Default signal handler |
| `SIG_IGN` | Ignore signal |
| `SIG_ERR` | Error return value |

---

## 💡 Quick Usage Patterns for Minitalk

### Server Implementation Pattern:
```c
int main(void)
{
    // Display server PID
    ft_putstr("Server PID: ");
    ft_putnbr(getpid());
    ft_putchar('\n');
    
    // Register signal handlers
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);
    
    // Wait for signals indefinitely
    while (1)
        pause();
        
    return (0);
}
```

### Client Implementation Pattern:
```c
void send_bit(int pid, int bit)
{
    if (bit == 0)
        kill(pid, SIGUSR1);    // Send bit 0
    else
        kill(pid, SIGUSR2);    // Send bit 1
        
    usleep(100);               // Prevent signal collision
}
```

### Error Handling Pattern:
```c
if (kill(pid, signal) == -1)
{
    ft_putstr("Error: Could not send signal\n");
    return (1);
}
```

---

## ⚠️ Important Notes

### Signal Safety:
- Only async-signal-safe functions should be used in signal handlers
- `write()` is safe, but `printf()` is not
- Static variables are safe to use in signal handlers

### Timing Considerations:
- Always use `usleep()` between signals to prevent collision
- Typical delay: 100-1000 microseconds
- Too fast: signals may be lost
- Too slow: unnecessary delay

### Error Handling:
- Always check return values of `kill()` calls
- Handle invalid PIDs gracefully
- Validate command line arguments

---

## 🔍 Debugging Tips

1. **Signal Handler Issues:**
   ```c
   // Add debug output in signal handler
   write(1, "Signal received\n", 16);
   ```

2. **PID Validation:**
   ```c
   if (pid <= 0)
   {
       ft_putstr("Invalid PID\n");
       return (1);
   }
   ```

3. **Timing Problems:**
   ```c
   // Increase delay if signals are lost
   usleep(1000);  // 1ms instead of 100μs
   ```

---

**This reference covers all essential functions for implementing a robust Minitalk project!** 🚀