/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2017, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
 */
#include "rtos-{{variant}}.h"
{{#timeout_tests}}
#include "machine-timer.h"
{{/timeout_tests}}
#include "debug.h"

#define DEMO_PRODUCTION_LIMIT 10
{{#timeout_tests}}
#define PART_3_SLEEP 3
#define PART_4_SLEEP 3

bool
tick_irq(void)
{
    machine_timer_tick_isr();

    rtos_timer_tick();

    return true;
}
{{/timeout_tests}}

void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;)
    {
    }
}

void
fn_a(void)
{
    int i;

    /* Part 0: Solo */
    debug_println("");
    debug_println("Part 0: Solo");
    debug_println("");

    debug_println("a: initializing maximum");
    rtos_sem_max_init(RTOS_SEM_ID_SEM0, DEMO_PRODUCTION_LIMIT);

    debug_println("a: V");
    rtos_sem_post(RTOS_SEM_ID_SEM0);

    debug_println("a: P (should succeed)");
    rtos_sem_wait(RTOS_SEM_ID_SEM0);
    debug_println("a: trying P (should fail)");
    if (rtos_sem_try_wait(RTOS_SEM_ID_SEM0))
    {
        debug_println("a: P try unexpectedly succeeded!");
    }
    debug_println("a: V");
    rtos_sem_post(RTOS_SEM_ID_SEM0);

    debug_println("a: trying P (should succeed)");
    if (!rtos_sem_try_wait(RTOS_SEM_ID_SEM0))
    {
        debug_println("a: P try unexpectedly failed!");
    }

    /* Part 1: B unblocks A */
    debug_println("");
    debug_println("Part 1: B unblocks A");
    debug_println("");

    debug_println("a: initializing maximum");
    rtos_sem_max_init(RTOS_SEM_ID_SEM1, DEMO_PRODUCTION_LIMIT);

    debug_println("a: P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_SEM1);
    debug_println("a: now runnable");

    debug_println("a: consuming...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("a: P");
        rtos_sem_wait(RTOS_SEM_ID_SEM1);
    }
    debug_println("a: trying P (should fail)");
    if (rtos_sem_try_wait(RTOS_SEM_ID_SEM1))
    {
        debug_println("a: P try unexpectedly succeeded!");
    }

    debug_println("a: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_ID_DEMO_HELPER);

    debug_println("a: consuming...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("a: P");
        rtos_sem_wait(RTOS_SEM_ID_SEM1);
    }
    debug_println("a: trying P (should fail)");
    if (rtos_sem_try_wait(RTOS_SEM_ID_SEM1))
    {
        debug_println("a: P try unexpectedly succeeded!");
    }

    /* Part 2: A and B compete */
    debug_println("");
    debug_println("Part 2: A and B compete");
    debug_println("");

    debug_println("a: initializing maximum");
    rtos_sem_max_init(RTOS_SEM_ID_SEM2, DEMO_PRODUCTION_LIMIT);

    debug_println("a: P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_SEM2);

    debug_println("a: should wake up before b. P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_SEM2);

    debug_println("a: should again wake up before b. waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_ID_DEMO_HELPER);

{{#timeout_tests}}
    /* Part 3: A's wait attempt times out */
    debug_println("");
    debug_println("Part 3: A's wait attempt times out");
    debug_println("");

    debug_println("a: initializing maximum");
    rtos_sem_max_init(RTOS_SEM_ID_SEM3, DEMO_PRODUCTION_LIMIT);

    debug_println("a: P (should time out)");
    if (rtos_sem_wait_timeout(RTOS_SEM_ID_SEM3, PART_3_SLEEP - 1)) {
        debug_println("a: P unexpectedly succeeded before timeout!");
    }
    debug_println("a: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_ID_DEMO_HELPER);
    debug_println("a: trying P (should succeed)");
    if (!rtos_sem_try_wait(RTOS_SEM_ID_SEM3))
    {
        debug_println("a: P try unexpectedly failed!");
    }

    /* Part 4: A's wait returns before timeout */
    debug_println("");
    debug_println("Part 4: A's wait returns before timeout");
    debug_println("");

    debug_println("a: initializing maximum");
    rtos_sem_max_init(RTOS_SEM_ID_SEM4, DEMO_PRODUCTION_LIMIT);

    debug_println("a: P (should succeed before timeout)");
    if (!rtos_sem_wait_timeout(RTOS_SEM_ID_SEM4, PART_4_SLEEP + 1)) {
        debug_println("a: P unexpectedly timed out!");
    }
{{/timeout_tests}}

    /* Part 5: A posts past maximum and triggers fatal error */
    debug_println("");
    debug_println("Part 5: A posts past maximum and triggers fatal error");
    debug_println("");

    debug_println("a: initializing maximum");
    rtos_sem_max_init(RTOS_SEM_ID_SEM5, DEMO_PRODUCTION_LIMIT);

    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("a: P");
        rtos_sem_post(RTOS_SEM_ID_SEM5);
    }

    debug_println("a: trying P (should trigger fatal error)");
    rtos_sem_post(RTOS_SEM_ID_SEM5);

    debug_println("a: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_b(void)
{
    int i;

    /* Part 1: B unblocks A */
    debug_println("b: V (should unblock a)");
    rtos_sem_post(RTOS_SEM_ID_SEM1);

    debug_println("b: producing while a consumes...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("b: V");
        rtos_sem_post(RTOS_SEM_ID_SEM1);
    }

    debug_println("b: producing while a is blocked...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("b: V");
        rtos_sem_post(RTOS_SEM_ID_SEM1);
    }

    debug_println("b: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_ID_DEMO_HELPER);

    /* Part 2: A and B compete */
    debug_println("b: P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_SEM2);

    debug_println("b: finally awake. sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_ID_DEMO_HELPER);

{{#timeout_tests}}
    /* Part 3: A's wait attempt times out */
    debug_println("b: sleeping");
    rtos_sleep(PART_3_SLEEP);
    debug_println("b: V (should not unblock a)");
    rtos_sem_post(RTOS_SEM_ID_SEM3);
    debug_println("b: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_ID_DEMO_HELPER);

    /* Part 4: A's wait returns before timeout */
    debug_println("b: sleeping");
    rtos_sleep(PART_4_SLEEP);
    debug_println("b: V (should unblock a)");
    rtos_sem_post(RTOS_SEM_ID_SEM4);
{{/timeout_tests}}

    debug_println("b: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_z(void)
{
    /* Part 2: A and B compete */
    debug_println("z: V");
    rtos_sem_post(RTOS_SEM_ID_SEM2);
    debug_println("z: V");
    rtos_sem_post(RTOS_SEM_ID_SEM2);
    debug_println("z: V");
    rtos_sem_post(RTOS_SEM_ID_SEM2);

{{#timeout_tests}}
    /* Part 3: A's wait attempt times out */
    debug_println("z: sleeping");
    rtos_sleep(PART_3_SLEEP);

    /* Part 4: A's wait returns before timeout */
    debug_println("z: sleeping");
    rtos_sleep(PART_4_SLEEP);
{{/timeout_tests}}

    debug_println("z: shouldn't be here!");
    for (;;)
    {
    }
}

int
main(void)
{
{{#timeout_tests}}
    machine_timer_start((void (*)(void))tick_irq);
{{/timeout_tests}}

    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
