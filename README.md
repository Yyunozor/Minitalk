# 📡 Minitalk

![Language](https://img.shields.io/badge/Language-C-blue)
![Status](https://img.shields.io/badge/Status-Completed-success)

## 🌟 Introduction

Minitalk is an inter-process communication system using UNIX signals. The client sends messages to the server using only `SIGUSR1` and `SIGUSR2` signals.

```
  CLIENT                              SERVER
+--------+                          +--------+
|        | --SIGUSR1 (bit 0)------> |        |
|        | --SIGUSR2 (bit 1)------> |        |
+--------+                          +--------+
```

## 🛠️ Key Concepts

### Binary Signal Communication

```
ASCII 'A' (65) = 01000001 in binary
  ┌───┬───┬───┬───┬───┬───┬───┬───┐
  │ 0 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │
  └─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┘
    │   │   │   │   │   │   │   │
    ▼   ▼   ▼   ▼   ▼   ▼   ▼   ▼
  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐
  │SIGUSR1│ │SIGUSR2│ │SIGUSR1│ │SIGUSR2│
  └─────┘ └─────┘ └─────┘ └─────┘
```

### How Characters are Transmitted

1. Each character → 8 bits
2. Each bit → signal (`SIGUSR1` for 0, `SIGUSR2` for 1)
3. Server reconstructs bits → character

## 📊 Implementation Details

### Client Side

#### 1. Sending a Bit (`s_bit` function)

```c
void s_bit(int pid, int bit)
{
    if (bit == 0)
        kill(pid, SIGUSR1);  // Send SIGUSR1 for bit 0
    else
        kill(pid, SIGUSR2);  // Send SIGUSR2 for bit 1
    usleep(BIT_DELAY);       // Wait to prevent signal overlap
}
```

```
Visualizing s_bit function:
┌─────────────┐          ┌─────────────┐
│ bit value 0 │──────────►  SIGUSR1    │─────► To Server
└─────────────┘          └─────────────┘
      │
      │                  ┌─────────────┐
      └──────────────────►  SIGUSR2    │─────► To Server
                         └─────────────┘
                            bit value 1
```

#### 2. Sending a Character (`s_char` function)

```c
void s_char(int pid, char c)
{
    int i = 7;
    while (i >= 0)
    {
        s_bit(pid, (c >> i) & 1);  // Extract each bit and send it
        i--;
    }
}
```

```
Character 'A' (65) = 01000001 in binary:

bit extraction: (c >> i) & 1
┌───────────────┬───────────────┬───────────────┐
│ i=7: c >> 7   │ 00000000      │ & 1 = 0      │
├───────────────┼───────────────┼───────────────┤
│ i=6: c >> 6   │ 00000001      │ & 1 = 1      │
├───────────────┼───────────────┼───────────────┤
│ i=5: c >> 5   │ 00000000      │ & 1 = 0      │
├───────────────┼───────────────┼───────────────┤
│ i=4: c >> 4   │ 00000000      │ & 1 = 0      │
├───────────────┼───────────────┼───────────────┤
│ i=3: c >> 3   │ 00000000      │ & 1 = 0      │
├───────────────┼───────────────┼───────────────┤
│ i=2: c >> 2   │ 00000000      │ & 1 = 0      │
├───────────────┼───────────────┼───────────────┤
│ i=1: c >> 1   │ 00000000      │ & 1 = 0      │
├───────────────┼───────────────┼───────────────┤
│ i=0: c >> 0   │ 01000001      │ & 1 = 1      │
└───────────────┴───────────────┴───────────────┘
```

#### 3. Client Main Function

```c
int main(int argc, char **argv)
{
    int		pid;
    char	*str;
    int		i;

    if (argc != 3)
    {
        ft_putstr("Usage: ./client <server_pid> <message>\n");
        return (1);
    }
    pid = ft_atoi(argv[1]);
    str = argv[2];
    i = 0;
    while (str[i])
    {
        s_char(pid, str[i]);
        i++;
    }
    s_char(pid, '\0');
    return (0);
}
```

```
Message sending process:
┌──────────────┐      ┌──────────┐      ┌────────────┐
│ Command Line │ ──► │ Get PID  │ ──► │ Get Message │
└──────────────┘      └──────────┘      └────────────┘
                                              │
                                              ▼
┌───────────────────────────────────────────────────────────┐
│                   For each character                      │
└───────────────────────────────────────────────────────────┘
                              │
                              ▼
┌───────────────────┐      ┌────────────────────────────────┐
│  s_char function  │ ◄── │ Send one char (8 bits) at a time│
└───────────────────┘      └────────────────────────────────┘
          │                                 │
          ▼                                 ▼
┌───────────────────┐      ┌────────────────────────────────┐
│  s_bit function   │ ◄── │    Send each bit as a signal    │
└───────────────────┘      └────────────────────────────────┘
          │
          ▼
┌───────────────────────────────────────────────────────────┐
│ Finally send NULL character to terminate message          │
└───────────────────────────────────────────────────────────┘
```

### Server Side

#### 1. Signal Handler (`x_signal` function)

```c
void x_signal(int signum)
{
    static int bit_count = 0;
    static unsigned char c = 0;
    static char buf[BUF_SIZE];
    static int idx = 0;

    c <<= 1;                    // Shift left to make room for new bit
    if (signum == SIGUSR2)      // If SIGUSR2, set the last bit to 1
        c |= 1;
    bit_count++;                // Count received bits
    
    if (bit_count == 8)         // If we've received a full byte (8 bits)
    {
        buf[idx++] = c;         // Store character in buffer
        if (c == '\0')          // End of message reached
        {
            ft_putstr(buf);     // Print the full message
            idx = 0;            // Reset buffer index
        }
        bit_count = 0;          // Reset bit counter
        c = 0;                  // Reset character
    }
}
```

```
Signal processing visualization:

Incoming signals:  SIGUSR1(0)  SIGUSR2(1)  SIGUSR1(0)  ...
                       │           │           │
                       ▼           ▼           ▼
Current byte:      0000000(0) → 0000001(0) → 0000010(0) ...

For each SIGUSR1: Left shift + bit remains 0
For each SIGUSR2: Left shift + rightmost bit becomes 1

Example reconstruction of letter 'A' (01000001):
┌─────────┬─────────────┬──────────────────┐
│ Signal  │ Operation   │ Result (c value) │
├─────────┼─────────────┼──────────────────┤
│ Initial │ -           │ 00000000         │
├─────────┼─────────────┼──────────────────┤
│ SIGUSR1 │ c <<= 1     │ 00000000         │
├─────────┼─────────────┼──────────────────┤
│ SIGUSR2 │ c <<= 1;    │ 00000001         │
│         │ c |= 1      │                  │
├─────────┼─────────────┼──────────────────┤
│ SIGUSR1 │ c <<= 1     │ 00000010         │
├─────────┼─────────────┼──────────────────┤
│ ...     │ ...         │ ...              │
├─────────┼─────────────┼──────────────────┤
│ SIGUSR2 │ c <<= 1;    │ 01000001 = 'A'   │
│         │ c |= 1      │                  │
└─────────┴─────────────┴──────────────────┘
```

#### 2. Server Main Function

```c
int main(void)
{
    print_pid();                      // Display server PID
    signal(SIGUSR1, x_signal);        // Set up signal handlers
    signal(SIGUSR2, x_signal);
    while (1)
        pause();                      // Wait for signals
    return (0);
}
```

```ascii
Server initialization and operation:
┌────────────────┐
│  Start server  │
└────────────────┘
        │
        ▼
┌────────────────┐
│  Get and show  │
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

## 🚀 Installation and Usage

1. Compile with `make`
2. Start the server: `./server`
3. Note the displayed server PID
4. Send a message: `./client [server_pid] "Your message here"`

## 📚 Lessons

This project demonstrates:

- Binary data representation
- Inter-process communication with signals
- Bit manipulation techniques
- Signal handling in C

The key insight is that even with just two signals (effectively a binary language), we can transmit any type of data between processes.