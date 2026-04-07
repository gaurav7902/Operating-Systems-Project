# Message Passing IPC --Gaurav

This project adds message-passing inter-process communication to xv6.

The implementation introduces two new system calls, `sendmsg` and `recvmsg`, and backs them with a per-process single-slot mailbox in the kernel. The design uses blocking semantics so that communication behaves like a simple synchronous IPC mechanism.

## Overview

Each process owns one mailbox slot.

- `sendmsg` places one message into the target process's mailbox.
- `recvmsg` removes one message from the caller's mailbox.
- If the mailbox is full, `sendmsg` blocks until the receiver consumes the message.
- If the mailbox is empty, `recvmsg` blocks until a sender provides data.

This keeps the implementation compact while still demonstrating synchronization, sleep/wakeup behavior, and kernel-managed IPC.

## System Call Interface

### `sendmsg`

User-space prototype:

```c
int sendmsg(int pid, void *msg, int len);
```

Description:

- Sends `len` bytes from the calling process to process `pid`.
- Copies data from the sender's address space into the receiver's mailbox.
- Returns `0` on success.
- Returns `-1` on invalid input or failure.

Behavior:

- Rejects messages larger than `MSGSIZE`.
- Blocks while the destination mailbox is full.
- Wakes the receiver after storing a message.

### `recvmsg`

User-space prototype:

```c
int recvmsg(int *src_pid, void *buf, int maxlen);
```

Description:

- Receives the pending mailbox message for the calling process.
- Copies the sender pid into `*src_pid`.
- Copies up to `maxlen` bytes into `buf`.
- Returns the number of bytes copied.
- Returns `-1` on invalid input or failure.

Behavior:

- Blocks while the mailbox is empty.
- Clears the mailbox after a successful receive.
- Wakes any blocked sender after consuming the message.

## Kernel Changes

The mailbox is stored directly in `struct proc`.

Added fields:

- `msg_lock` for synchronization
- `mailbox_full` to indicate whether the slot is occupied
- `mailbox_src_pid` to record the sender
- `mailbox_len` to store the payload size
- `mailbox_data[MSGSIZE]` to store the payload bytes

`MSGSIZE` is defined as `128` bytes.

Initialization and cleanup:

- Mailbox fields are initialized in `allocproc`.
- Mailbox state is cleared again in `freeproc`.

## Blocking Model

The mailbox uses xv6 sleep/wakeup primitives to coordinate senders and receivers.

- A sender sleeps while the receiver's mailbox is full.
- A receiver sleeps while its mailbox is empty.
- Successful send operations wake the waiting receiver.
- Successful receive operations wake waiting senders.

This provides a deterministic one-message-at-a-time communication path.

## Files Modified

### Kernel

- `xv6/kernel/proc.h` - added mailbox fields to `struct proc`
- `xv6/kernel/proc.c` - initialized and cleared mailbox state
- `xv6/kernel/sysproc.c` - implemented `sys_sendmsg` and `sys_recvmsg`
- `xv6/kernel/syscall.h` - added syscall numbers
- `xv6/kernel/syscall.c` - registered syscall handlers

### User Space

- `xv6/user/user.h` - added user-space prototypes
- `xv6/user/usys.pl` - generated syscall stubs
- `xv6/user/mailboxtest.c` - test program for blocking IPC

### Build System

- `xv6/Makefile` - added `_mailboxtest` to the user program list

## Test Program

The user test program `mailboxtest` demonstrates the IPC flow.

Scenario:

1. Parent sends the first message.
2. Parent attempts to send a second message and blocks if the mailbox is still occupied.
3. Child receives the first message.
4. The blocked sender is unblocked.
5. Child receives the second message.

This confirms that the mailbox enforces one-slot communication and proper blocking behavior.

## Build and Run

From the `xv6/` directory:

```bash
make qemu
```

Inside the xv6 shell:

```bash
mailboxtest
```

Expected output shows the sender pid and the two messages being transferred in order.

## Notes

- The mailbox is intentionally a single-slot buffer rather than a queue.
- The design is focused on clarity and synchronization, not throughput.
- This implementation is suitable for demonstrating core IPC concepts in xv6.

# 📌 Project 1 – xv6 System Call Customization  (getprocinfo) --John

## 👤 Contribution Scope

This module focuses on **process inspection and metadata enhancement** in the xv6 operating system. It introduces new system calls and extends process structures to enable user-space programs to retrieve detailed information about running processes.

---

## 🚀 Implemented Features

### 1. `getppid()` System Call

Returns the parent process ID of the calling process.

**Purpose:**

* Understand process hierarchy
* Validate parent-child relationships

**Example Output:**

```
PID: 3, PPID: 2
```

---

### 2. `getprocinfo(int pid, struct procinfo *info)` System Call

Retrieves detailed information about a specific process.

**Fields Returned:**

* `pid` → Process ID
* `state` → Process state (RUNNING, SLEEPING, etc.)
* `sz` → Memory size
* `parent_pid` → Parent process ID
* `priority` → Process priority

---

## 🧠 Data Structures

### Kernel Side (`kernel/proc.h`)

```c
struct procinfo {
    int pid;
    int state;
    int sz;
    int parent_pid;
    int priority;
};
```

### Process Structure Extension

```c
struct proc {
    ...
    int priority;   // Added field
};
```

---

## ⚙️ System Call Workflow

```
User Program (testproc.c)
        ↓
User Stub (usys.pl → usys.S)
        ↓
sys_getprocinfo() (sysproc.c)
        ↓
Process Table Traversal (proc[])
        ↓
copyout() → User Space
```

---

## 🧪 Test Program (`testproc.c`)

A user-level program was developed to:

* Invoke system calls
* Display process details in a formatted table (ps-like view)

### Sample Output

```
PID     STATE           SIZE    PARENT  PRIORITY
------------------------------------------
1       SLEEPING        16384   -1      5
2       SLEEPING        20480   1       5
3       RUNNING         16384   2       5
```

---

## 🔧 Key Implementation Details

### Argument Handling

* `argint()` → fetch integer arguments
* `argaddr()` → fetch pointer arguments

### Memory Transfer

* `copyout()` used to safely transfer data from kernel to user space

### Process Traversal

* Iteration over `proc[NPROC]` to locate matching PID

---

## ⚠️ Challenges Faced

1. **Kernel-User Data Transfer**

   * Direct struct return is not possible → solved using `copyout()`

2. **xv6-riscv Differences**

   * `argint()` and `argaddr()` return `void` (not int)

3. **Structure Synchronization**

   * Ensured identical `procinfo` struct in both kernel and user space

4. **Formatting Output**

   * Implemented aligned printing for readability

---

## 📈 Enhancements Made

* Added `priority` field to process structure
* Introduced human-readable process states
* Built a **ps-like process viewer** in user space

---

## 🧩 Learning Outcomes

* System call design and implementation
* Kernel-user boundary handling
* Process table management
* Safe memory operations (`copyout`)
* Struct synchronization across layers

---

## ✅ Conclusion

This module transforms xv6 from a basic OS into a more **introspective system**, enabling users to:

* Inspect process states
* Understand process relationships
* Analyze memory usage and scheduling attributes

The implementation demonstrates practical understanding of **operating system internals and system call mechanisms**.

---
