// alarm_test.c -- Demonstrates the alarm_signal() system call.
//
// This program registers a periodic alarm that fires every N ticks.
// Each time the alarm fires, the kernel diverts execution to the
// handler function, which increments a counter and prints a message.
// After the handler calls alarm_return(), the kernel restores the
// original execution state and the main loop continues.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Global counter: incremented each time the alarm handler fires.
volatile int alarm_count = 0;

// ----------------------------------------------------------------
// alarm_handler -- called by the kernel when the alarm fires.
//
// This function runs in user space. It MUST call alarm_return()
// at the end so the kernel can restore the saved trapframe and
// resume the interrupted code.
// ----------------------------------------------------------------
void alarm_handler(void)
{
  alarm_count++;
  printf(">>> ALARM FIRED! (count = %d) <<<\n", alarm_count);
  alarm_return();
}

int
main(int argc, char *argv[])
{
  printf("==============================================\n");
  printf("   alarm_test: Alarm Signal System Call Demo\n");
  printf("==============================================\n\n");

  // ---- Test 1: Periodic alarm with 5-tick interval ----
  printf("[Test 1] Setting alarm_signal(5, alarm_handler)...\n");
  printf("         The handler will fire every 5 timer ticks.\n\n");

  alarm_signal(5, alarm_handler);

  // Spin in a busy loop until the alarm has fired 3 times.
  // The kernel will interrupt this loop on timer ticks.
  printf("Entering busy loop. Waiting for 3 alarm firings...\n\n");
  while (alarm_count < 3) {
    // Busy-wait: the timer interrupt will fire the alarm.
  }

  printf("\n[Test 1 PASSED] Alarm fired %d times as expected!\n\n", alarm_count);

  // ---- Test 2: Disable the alarm ----
  printf("[Test 2] Disabling alarm with alarm_signal(0, 0)...\n");
  alarm_signal(0, 0);

  int saved = alarm_count;

  // Spin for a while; alarm should NOT fire.
  for (int i = 0; i < 500000000; i++) {
    // busy loop
  }

  if (alarm_count == saved) {
    printf("[Test 2 PASSED] Alarm did not fire after being disabled.\n\n");
  } else {
    printf("[Test 2 FAILED] Alarm fired %d extra time(s)!\n\n",
           alarm_count - saved);
  }

  // ---- Test 3: Different interval (10 ticks) ----
  printf("[Test 3] Setting alarm_signal(10, alarm_handler)...\n");
  printf("         Waiting for 2 more alarm firings...\n\n");

  int target = alarm_count + 2;
  alarm_signal(10, alarm_handler);

  while (alarm_count < target) {
    // busy wait
  }

  printf("[Test 3 PASSED] Alarm fired with 10-tick interval!\n\n");

  // Clean up
  alarm_signal(0, 0);

  printf("==============================================\n");
  printf("   ALL TESTS PASSED! alarm_signal works!\n");
  printf("==============================================\n");

  exit(0);
}
