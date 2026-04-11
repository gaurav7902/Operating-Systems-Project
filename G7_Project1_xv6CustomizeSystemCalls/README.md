## Syscall Logger Enhancement
This project implements a real-time syscall logging mechanism that captures system calls as they occur and prints them immediately to the console. This is crucial for debugging and understanding how user-space processes interact with the kernel.

### What this syscall() function does actually
In the context of xv6, the syscall() function acts like a traffic controller. It is the central junction that takes a request from the user program and routes it to the correct part of the operating system kernel. Here is the breakdown of what it does step-by-step:
1. **It identifies the request:** When a user program (like our syscall logger) wants the kernel to do something, it puts a **System call number** into a specific CPU register. The syscall() function reads this number from the process's Trap Frame.
2. **It validates the call:** The function checks if the provided number is valid.
   * **If valid:** It uses that number as an index to look up a function pointer in a table (usually called syscalls[]).
   * **If invalid:** It prints an error message (like "unknown syscall") and returns -1.
3. **It executes and stores the result:** Once it finds the correct internal function (like sys_fork, sys_write, or any others), it runs that function.
   After the kernel function finished, syscall() takes the return value and saves it back into the register of the trap frame so the user program can see if the operation succeeded or failed.

### Enhancing Real-time logging (What was modified):
* Originally, the system might have buffered logs or only recorded them silently. This update introduces **Synchronous Logging**, ensuring that as soon as a process triggers a syscall, the details are visible on the screen.

* The logic that was added basically says: **"Right before you route this request to the kernel, print out what is happening to the console so I can see it in real-time.**

* **Key Modifications:**
1. **Immediate flush:** Modified the logging core to bypass or flush buffers,  ensuring no delay between the event and the output.
2. **Kernel-Level Hooks:** Integrated the printf logic directly into the **syscall()** function within the kernel.
3. **Traceability:** Each log entry displays the system call being executed, providing a live trace of system activity.

### System Verification (Screenshots) 
* **Kernel Boot Sequence:** When the xv6 kernel boots, the system initiates several background processes like **init** and **sh**. This logger captures these initial handshakes between the hardware and software.

<img width="1920" height="981" alt="Screenshot (1)" src="https://github.com/user-attachments/assets/4fade284-fe0b-4f48-80c2-a0d11a732bd8" />
The termial showing syscalls being logged automatically during the xv6 boot process.

* **User Program Execution:** Beyond boot-up, the logger also tracks specific user-run programs. This allows developers to see exactly how many **read**, **write** or **fork** calls a simple command actually triggers.
<img width="1920" height="939" alt="Screenshot (6)" src="https://github.com/user-attachments/assets/4956dd7f-85ca-4c80-be61-b09d91156b23" />
 
### How to Test
To see the real-time logging in action:
1. **Clean and Rebuild:**
   ```bash
   make clean
   make qemu

2. **Observe the Boot:** Watch the console as init and sh start; you will see syscall logs appearing immediately.
3. **Run a User Command:** In the xv6 shell, run a command like ls.
4. **Verify output:** Each syscall involved in running  that command will be printed to the terminal in real-time.

**By implementing this real-time logger, we've turned the "Black box" of the xv6 kernel into transparent environment. This makes it an excellent tool for students and developers to visualize the underlying mechanics of operating system resource management.**
