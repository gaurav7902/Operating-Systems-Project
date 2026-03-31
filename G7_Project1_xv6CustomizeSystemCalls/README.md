# 📌 Project 1 – xv6 System Call Customization (getprocinfo)

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
