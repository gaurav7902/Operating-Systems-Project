# Alarm Signal — xv6 Custom System Call Project  
**Theme:** Kernel-level Process Control and Signal Handling
---
## Project Overview
This project implements the **`alarm_signal(int ticks, void (*handler)())`** system call in the xv6 operating system. It bridges the gap between hardware timer interrupts and user-space execution by allowing a process to register a callback function that the kernel automatically invokes after a specified number of timer ticks.

### What Does This Project Do? (Simple Explanation)

Imagine you set a kitchen timer for 5 minutes. You go back to cooking (your main work). When the timer rings, you stop cooking, go check the oven (your handler task), and then return to cooking exactly where you left off.

**That is exactly what `alarm_signal` does — but inside an operating system:**

1. A user program tells the kernel: *"Every 5 timer ticks, call this function for me."*
2. The kernel keeps counting ticks in the background while the program runs normally.
3. When 5 ticks pass, the kernel **pauses** the program, **runs the handler function**, and then **resumes** the program as if nothing happened.
4. This repeats automatically. To stop it, the program calls `alarm_signal(0, 0)`.

### Key Terms

- **Tick** — One hardware timer interrupt. The CPU generates these at a fixed rate (~10 per second in xv6). Each tick is the kernel's "heartbeat."
- **Firing** — When the alarm "goes off." After counting down the specified number of ticks, the alarm **fires** — meaning the kernel redirects the process to run the handler function. Each firing produces one `>>> ALARM FIRED! <<<` message.
- **Busy Loop** — A loop that keeps the CPU busy doing nothing useful (`while(condition) {}`). The test program uses this to keep the process running while waiting for the timer to fire. Without a busy loop, the program would exit before any alarms could fire.
- **Handler** — A user-written function that the kernel calls when the alarm fires. It must call `alarm_return()` at the end to go back to the main program.

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

## Overall Process — Step by Step

Here is the complete flow of how the alarm works from start to finish:

### Step 1: User Program Registers the Alarm
```c
alarm_signal(5, alarm_handler);  // "Call alarm_handler every 5 ticks"
```
The kernel stores these values inside the process's `struct proc`:
- `alarm_interval = 5` (the repeat interval)
- `alarm_handler = address of alarm_handler function`
- `alarm_ticks_left = 5` (countdown starts)

### Step 2: Program Continues Running (Busy Loop)
```c
while (alarm_count < 3) {
    // The program spins here doing nothing.
    // Meanwhile, the hardware timer keeps ticking.
}
```
The program runs this loop normally. Every ~0.1 seconds, a **timer interrupt** fires and the CPU enters the kernel.

### Step 3: Kernel Counts Down on Each Timer Tick
Inside `usertrap()` in `kernel/trap.c`, on every timer interrupt:
```
tick 1: alarm_ticks_left = 4  → not yet, keep going
tick 2: alarm_ticks_left = 3  → not yet
tick 3: alarm_ticks_left = 2  → not yet
tick 4: alarm_ticks_left = 1  → not yet
tick 5: alarm_ticks_left = 0  → ALARM FIRES!
```

### Step 4: Alarm Fires — Kernel Saves State and Redirects
When the countdown reaches 0:
1. The kernel **saves the entire trapframe** (all 32 CPU registers + program counter) into `alarm_saved_tf`
2. The kernel **overwrites the program counter** (`epc`) to point to `alarm_handler`
3. Sets `alarm_active = 1` to prevent double-firing
4. Returns to user space → the CPU now executes `alarm_handler` instead of the busy loop

### Step 5: Handler Runs and Returns
```c
void alarm_handler(void) {
    alarm_count++;                                          // Increment counter
    printf(">>> ALARM FIRED! (count = %d) <<<\n", alarm_count); // Print message
    alarm_return();                                         // Tell kernel: "I'm done"
}
```
When `alarm_return()` is called:
1. The kernel **restores the saved trapframe** back into the process
2. Resets `alarm_ticks_left = 5` (start counting again)
3. Clears `alarm_active = 0` (allow future alarms)
4. The process **resumes the busy loop** exactly where it was interrupted

### Step 6: Repeat
The cycle repeats. After 5 more ticks, the alarm fires again (count = 2), then again (count = 3). When `alarm_count` reaches 3, the busy loop's condition `alarm_count < 3` becomes false, and the program moves on to Test 2.

---

## How the Kernel Does It — Trapframe Save/Restore

The **trapframe** is the critical data structure. It holds all 32 CPU registers and the program counter. Here's the flow diagram:

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
│  - Prints ">>> ALARM FIRED! <<<" message                    │
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

## Files Modified and What Each Change Does

| # | File | What Was Changed | Why |
|---|------|-----------------|-----|
| 1 | `kernel/proc.h` | Added 5 new fields to `struct proc` | Each process needs its own alarm state (interval, handler, countdown, guard flag, saved registers) |
| 2 | `kernel/proc.c` | Initialize alarm fields in `allocproc()`, cleanup in `freeproc()`, copy in `kfork()` | When a process is created, its alarm must start as disabled; when it forks, the child inherits the alarm; when it exits, the alarm memory must be freed |
| 3 | `kernel/syscall.h` | Added `#define SYS_alarm_signal 22` and `#define SYS_alarm_return 23` | Every system call needs a unique number so the kernel can identify which call the user is making |
| 4 | `kernel/syscall.c` | Added `sys_alarm_signal` and `sys_alarm_return` to the dispatch table | The kernel uses this table to map system call numbers to their handler functions |
| 5 | `kernel/sysproc.c` | Implemented `sys_alarm_signal()` and `sys_alarm_return()` functions | These are the actual kernel functions that set up the alarm and restore the trapframe |
| 6 | `kernel/trap.c` | Added alarm countdown + trapframe save/redirect logic in `usertrap()` | This is the **core logic** — on every timer tick, check if alarm should fire and redirect the process |
| 7 | `user/user.h` | Added function declarations for `alarm_signal()` and `alarm_return()` | User programs need to know these functions exist so they can call them |
| 8 | `user/usys.pl` | Added assembly stub entries | Generates the assembly code that makes the actual `ecall` instruction to enter the kernel |
| 9 | `user/alarm_test.c` | **NEW FILE** — Test program with 3 tests | Demonstrates and validates the alarm system call works correctly |
| 10 | `Makefile` | Added `_alarm_test` to `UPROGS` list | Tells the build system to compile the test program and include it in the xv6 disk image |

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

This is the **most important code** — it runs on every timer interrupt:

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

**Line-by-line explanation:**
- `which_dev == 2` → This was a timer interrupt (not keyboard or disk)
- `p->alarm_interval > 0` → This process has an alarm set
- `p->alarm_active == 0` → The handler is not already running
- `p->alarm_ticks_left--` → Count down one tick
- `memmove(...)` → Copy all registers to the backup
- `p->trapframe->epc = p->alarm_handler` → Change where the process will resume — instead of returning to the busy loop, it will jump to the handler
- `p->alarm_active = 1` → Block further alarms until handler finishes

### 3. Trapframe Restore in `sys_alarm_return()` (`kernel/sysproc.c`)

```c
uint64 sys_alarm_return(void) {
  struct proc *p = myproc();
  // Copy the saved registers back, restoring the exact CPU state
  memmove(p->trapframe, p->alarm_saved_tf, sizeof(struct trapframe));
  // Start counting ticks again for the next alarm
  p->alarm_ticks_left = p->alarm_interval;
  // Allow alarms to fire again
  p->alarm_active = 0;
  return p->trapframe->a0;
}
```

### 4. The Alarm Handler in User Space (`user/alarm_test.c`)

This is the code that prints `">>> ALARM FIRED! (count = 1) <<<"`:

```c
// Global counter — starts at 0, incremented each time the alarm fires
volatile int alarm_count = 0;

void alarm_handler(void)
{
  alarm_count++;    // 0→1 on first firing, 1→2 on second, 2→3 on third
  printf(">>> ALARM FIRED! (count = %d) <<<\n", alarm_count);
  alarm_return();   // MUST call this — tells kernel to restore saved state
}
```

**How the count works:**
- The variable `alarm_count` is **global** and marked `volatile` (so the compiler doesn't optimize away reads from it)
- First alarm firing: `alarm_count` goes from 0 → 1, prints `count = 1`
- Second alarm firing: `alarm_count` goes from 1 → 2, prints `count = 2`
- Third alarm firing: `alarm_count` goes from 2 → 3, prints `count = 3`
- The main busy loop checks `while (alarm_count < 3)` — once count reaches 3, the loop exits

---

## Test Program — Detailed Explanation

### Test 1: Periodic Alarm (5-tick interval, 3 firings)
```c
alarm_signal(5, alarm_handler);    // Set alarm for every 5 ticks
while (alarm_count < 3) { }       // Busy loop — wait for 3 firings
```
- **Why 3 firings?** We chose 3 to prove the alarm is **repeating** — it's not a one-time event. After each firing, `alarm_return()` resets the countdown and the alarm fires again. Three firings prove the cycle works correctly.

### Test 2: Disabling the Alarm
```c
alarm_signal(0, 0);                // Disable — set interval to 0
for (int i = 0; i < 500000000; i++) { }  // Spin for a long time
```
- After disabling, we spin for a long time. If no alarm fires during this period, the disable works correctly.

### Test 3: Different Interval (10-tick interval, 2 firings)
```c
alarm_signal(10, alarm_handler);   // Re-enable with 10 ticks
while (alarm_count < target) { }   // Wait for 2 more firings
```
- **Why only 2 firings for Test 3?** Because we already proved the alarm repeats in Test 1 (3 firings). Test 3's purpose is different — it proves the system call works with **any tick value**, not just 5. Two firings is enough to confirm the 10-tick interval works. The alarm count continues from where it left off (count 4 and 5).

---

## Execution Screenshot

The screenshot below shows `alarm_test` running inside xv6 on QEMU. All 3 tests pass successfully:

![alarm_test output showing all 3 tests passing in QEMU](alarm_test_output.png)

**What the output shows:**
- **Test 1**: Three `>>> ALARM FIRED! <<<` messages appear at count 1, 2, 3 — proving the 5-tick periodic alarm works
- **Test 2**: No alarm messages appear — proving the alarm was successfully disabled
- **Test 3**: Two more `>>> ALARM FIRED! <<<` messages at count 4, 5 — proving the 10-tick interval works
- **Final line**: `ALL TESTS PASSED! alarm_signal works!`

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

## Five System Call Functionalities Covered

This project satisfies the requirement of modifying/implementing system calls across multiple OS functionality areas:

1. **Signals** — `alarm_signal()` implements a SIGALRM-like timer signal mechanism
2. **Process Control** — The kernel tracks per-process alarm state and controls execution flow
3. **Inter-Process Communication** — The trapframe save/restore mechanism is a form of kernel-to-user signaling
4. **Process Creation** — `kfork()` was modified to properly inherit alarm state to child processes
5. **Locks / Concurrency** — The `alarm_active` re-entrancy guard prevents race conditions when the handler runs longer than the alarm interval
