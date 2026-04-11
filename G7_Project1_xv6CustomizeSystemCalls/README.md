## Syscall Logger Enhancement
Updated the syscall logging mechanism to provide real-time feedback. Previously, logs have been buffered; now, every syscall is printed immediately after it is captured.

### Changes Made
* **Synchronous Output:** Modified the logging core to flush the output buffer immediately after a syscall is recorded.
* **Real-time Printing:** Added a print hook directly following the log-writing logic to ensure current system activity without delay.
* Whenever the xv6 kernel boots, there are processes starting like init to be executed by the system. The syscalls for these are now printed:
<img width="1920" height="981" alt="Screenshot (1)" src="https://github.com/user-attachments/assets/4fade284-fe0b-4f48-80c2-a0d11a732bd8" />
Here is the screenshot for the user program of syscall logger:
<img width="1920" height="939" alt="Screenshot (6)" src="https://github.com/user-attachments/assets/4956dd7f-85ca-4c80-be61-b09d91156b23" />
 
