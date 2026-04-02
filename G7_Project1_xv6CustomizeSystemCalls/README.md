# Alarm Signal — xv6 Custom System Call Project

**Author:** Sathish  
**Branch:** `sathish-alarm-feature`  
**Theme:** Kernel-level Process Control and Signal Handling

---

## Project Overview

This project implements the **`alarm_signal(int ticks, void (*handler)())`** system call in the xv6 operating system. It bridges the gap between hardware timer interrupts and user-space execution by allowing a process to register a callback function that the kernel automatically invokes after a specified number of timer ticks.

### What It Does

- A user process calls `alarm_signal(N, handler_fn)` to register a timer alarm
- Every `N` timer ticks, the kernel **diverts the process's execution** to `handler_fn`
- The handler calls `alarm_return()` to **restore the original execution state**
- The alarm repeats automatically until disabled with `alarm_signal(0, 0)`

---

## System Calls Implemented

### 1. `alarm_signal(int ticks, void (*handler)())`

| Parameter | Description |
|-----------|-------------|
| `ticks`   | Number of timer interrupts between each alarm (0 to disable) |
| `handler` | Pointer to a user-space function to call when the alarm fires |
| **Returns** | 0 on success |

### 2. `alarm_return(void)`

| Description |
|-------------|
| Must be called at the end of the alarm handler to restore the process's original execution state |
| **Returns** | Restores the original return value from before the alarm interrupted |

---

## How It Works — Technical Explanation

### The Problem

When a timer interrupt occurs, the CPU stops the user process and enters the kernel via `usertrap()`. The kernel needs to:
1. **Count ticks** for each process that has an alarm set
2. When the countdown hits zero, **redirect the process to the handler** without losing the original execution state
3. After the handler finishes, **resume the process exactly where it was interrupted**

### The Solution: Trapframe Save/Restore

The key insight is the **trapframe** — a data structure that holds all 32 CPU registers plus the program counter (`epc`). Here's the flow:

```
┌─────────────────────────────────────────────────────────────┐
│  User Process Running (e.g., busy loop at address 0x1234)   │
└─────────────────────────────┬───────────────────────────────┘
                              │ Timer interrupt!
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  Kernel: usertrap()                                         │
│  1. Decrement alarm_ticks_left                              │
│  2. If reached zero:                                        │
│     a. SAVE entire trapframe → alarm_saved_tf               │
│     b. Set trapframe->epc = alarm_handler address           │
│     c. Set alarm_active = 1 (prevent re-entry)              │
└─────────────────────────────┬───────────────────────────────┘
                              │ Return to user space
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  User: alarm_handler() executes                             │
│  - Prints message, does work                                │
│  - Calls alarm_return()                                     │
└─────────────────────────────┬───────────────────────────────┘
                              │ System call
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  Kernel: sys_alarm_return()                                 │
│  1. RESTORE alarm_saved_tf → trapframe                      │
│  2. Reset alarm_ticks_left = alarm_interval                 │
│  3. Clear alarm_active = 0                                  │
└─────────────────────────────┬───────────────────────────────┘
                              │ Return to user space
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  User Process Resumes at original address 0x1234            │
│  (all registers restored — process doesn't know it          │
│   was interrupted!)                                         │
└─────────────────────────────────────────────────────────────┘
```

### Why `alarm_active` Is Needed

Without this guard flag, if the alarm handler takes longer than `N` ticks to execute, the kernel would try to fire the alarm *again* while the handler is still running—corrupting the saved trapframe. The `alarm_active` flag prevents this re-entrant scenario.

---

## Files Modified

| File | Change |
|------|--------|
| `kernel/proc.h` | Added 5 alarm fields to `struct proc` |
| `kernel/proc.c` | Initialize alarm in `allocproc()`, cleanup in `freeproc()`, copy in `kfork()` |
| `kernel/syscall.h` | Added `SYS_alarm_signal` (22) and `SYS_alarm_return` (23) |
| `kernel/syscall.c` | Registered both system calls in the dispatch table |
| `kernel/sysproc.c` | Implemented `sys_alarm_signal()` and `sys_alarm_return()` handlers |
| `kernel/trap.c` | Added alarm tick countdown and trapframe save/redirect in `usertrap()` |
| `user/user.h` | Added user-space function declarations |
| `user/usys.pl` | Added assembly stub entries |
| `user/alarm_test.c` | **NEW** — Test program demonstrating the alarm signal |
| `Makefile` | Added `_alarm_test` to `UPROGS` |

---

## Key Code Changes

### 1. `struct proc` — New Alarm Fields (`kernel/proc.h`)

```c
// Alarm signal fields
int alarm_interval;            // Timer interval in ticks (0 = disabled)
uint64 alarm_handler;          // User-space handler function address
int alarm_ticks_left;          // Countdown to next alarm firing
int alarm_active;              // Guard: 1 while handler is executing
struct trapframe *alarm_saved_tf; // Saved trapframe for alarm_return()
```

### 2. Alarm Check in `usertrap()` (`kernel/trap.c`)

```c
// On every timer tick, check if this process has a pending alarm.
if(which_dev == 2 && p->alarm_interval > 0 && p->alarm_active == 0) {
  p->alarm_ticks_left--;
  if(p->alarm_ticks_left <= 0) {
    // Save the entire trapframe so alarm_return() can restore it.
    memmove(p->alarm_saved_tf, p->trapframe, sizeof(struct trapframe));
    // Redirect the process to execute the alarm handler.
    p->trapframe->epc = p->alarm_handler;
    // Prevent re-entrant alarms while handler is running.
    p->alarm_active = 1;
  }
}
```

### 3. Trapframe Restore in `sys_alarm_return()` (`kernel/sysproc.c`)

```c
uint64 sys_alarm_return(void) {
  struct proc *p = myproc();
  memmove(p->trapframe, p->alarm_saved_tf, sizeof(struct trapframe));
  p->alarm_ticks_left = p->alarm_interval;
  p->alarm_active = 0;
  return p->trapframe->a0;
}
```

---

## Test Program — `alarm_test.c`

The test program runs 3 tests:

| Test | Description | Expected Result |
|------|-------------|-----------------|
| **Test 1** | Set alarm with 5-tick interval, wait for 3 firings | Handler prints 3 times, busy loop continues |
| **Test 2** | Disable alarm with `alarm_signal(0, 0)`, spin | No alarm fires after disabling |
| **Test 3** | Re-enable with 10-tick interval, wait for 2 firings | Handler fires with the new interval |

---

## Execution Screenshot

![alarm_test output showing all 3 tests passing in QEMU](alarm_test_output.png)

---

## How to Build and Run

```bash
# Navigate to the xv6 directory
cd G7_Project1_xv6CustomizeSystemCalls/xv6/

# Clean and build
make clean
make qemu CPUS=1

# Inside xv6 shell, run the test
$ alarm_test
```

---

## Five System Call Functionalities Covered

This project satisfies the requirement of modifying/implementing system calls across multiple OS functionality areas:

1. **Signals** — `alarm_signal()` implements a SIGALRM-like timer signal mechanism
2. **Process Control** — The kernel tracks per-process alarm state and controls execution flow
3. **Inter-Process Communication** — The trapframe save/restore mechanism is a form of kernel-to-user signaling
4. **Process Creation** — `kfork()` was modified to properly inherit alarm state to child processes
5. **Locks / Concurrency** — The `alarm_active` re-entrancy guard prevents race conditions when the handler runs longer than the alarm interval
