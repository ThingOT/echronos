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
#include "rtos-phact.h"
#include "machine-timer.h"
#include "debug.h"

#define PART_4_SLEEP 3
#define PART_5_SLEEP 3

bool
tick_irq(void)
{
    machine_timer_tick_isr();

    rtos_timer_tick();

    return true;
}

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
    /* Part 0: Solo */
    debug_println("");
    debug_println("Part 0: Solo");
    debug_println("");

    debug_println("a: taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("a: trying held lock");
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_M0))
    {
        debug_println("a: unexpected mutex not locked!");
    }
    debug_println("a: releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    debug_println("a: trying unheld lock");
    if (!rtos_mutex_try_lock(RTOS_MUTEX_ID_M0))
    {
        debug_println("a: unexpected mutex locked!");
    }
    debug_println("a: releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    /* Part 1: Mutex with ceiling higher than all tasks */
    debug_println("");
    debug_println("Part 1: Mutex with ceiling higher than all tasks");
    debug_println("");

    debug_println("a: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("a: should get signal 1st, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("a: should get signal first, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("a: got signal, taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("a: releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);

    /* Part 2: Mutex with ceiling lower than A's */
    debug_println("");
    debug_println("Part 2: Mutex with ceiling lower than A's");
    debug_println("");

    debug_println("a: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("a: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("a: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("a: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("a: got signal, done");

    /* Part 3: Two mutexes with differing ceilings */
    debug_println("");
    debug_println("Part 3: Two mutexes with differing ceilings");
    debug_println("");

    debug_println("a: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("a: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("a: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("a: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("a: got signal, done");

    /* Part 4: B's lock attempt times out */
    debug_println("");
    debug_println("Part 4: B's lock attempt times out");
    debug_println("");

    debug_println("a: taking the lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M4);
    debug_println("a: sleeping");
    rtos_sleep(PART_4_SLEEP);
    debug_println("a: releasing the lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M4);
    debug_println("a: signalling b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P4);
    debug_println("a: waiting until b has the lock");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P4);

    /* Part 5: B gets lock before timeout */
    debug_println("");
    debug_println("Part 5: B gets lock before timeout");
    debug_println("");

    debug_println("a: taking the lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M5);
    debug_println("a: sleeping");
    rtos_sleep(PART_5_SLEEP);
    debug_println("a: releasing the lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M5);
    debug_println("a: waiting until b has the lock");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P5);

    /* Part 6: Task takes mutex with lower priority ceiling */
    debug_println("");
    debug_println("Part 6: Task takes mutex with lower priority ceiling");
    debug_println("");
    debug_println("a: taking lock (should trigger fatal error)");
    rtos_mutex_lock(RTOS_MUTEX_ID_M6);

    debug_println("a: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_b(void)
{
    /* Part 1: Mutex with ceiling higher than all tasks */
    debug_println("b: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("b: should get signal 2nd, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("b: should get signal last, taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("b: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("b: still running at greater priority than a. releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);

    /* Part 2: Mutex with ceiling lower than A's */
    debug_println("b: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("b: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("b: got signal, taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M2);
    debug_println("b: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("b: releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M2);
    debug_println("b: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P2);

    /* Part 3: Two mutexes with differing ceilings */
    debug_println("b: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("b: got signal, waiting on lock H");
    rtos_mutex_lock(RTOS_MUTEX_ID_M3_H);
    debug_println("b: got lock H, sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("b: releasing lock H");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M3_H);
    debug_println("b: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("b: got signal, waiting again");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("b: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P3);

    /* Part 4: B's lock attempt times out */
    debug_println("b: blocking on the lock, should time out");
    if (rtos_mutex_lock_timeout(RTOS_MUTEX_ID_M4, PART_4_SLEEP - 1)) {
        debug_println("b: lock unexpectedly succeeded before timeout!");
    }
    debug_println("b: waiting for a to signal the lock's free");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P4);
    debug_println("b: taking the lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M4);
    debug_println("b: waking up a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P4);
    debug_println("b: releasing the lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M4);

    /* Part 5: B gets lock before timeout */
    debug_println("b: blocking on the lock, should succeed");
    if (!rtos_mutex_lock_timeout(RTOS_MUTEX_ID_M5, PART_5_SLEEP + 1)) {
        debug_println("b: lock unexpectedly timed out!");
    }
    debug_println("b: waking up a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P5);
    debug_println("b: releasing the lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M5);

    debug_println("b: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_y(void)
{
    /* Part 1: Mutex with ceiling higher than all tasks */
    debug_println("y: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("y: should get signal last, taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("y: sending signal to b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("y: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("y: still running at greater priority than a and b. releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);

    /* Part 2: Mutex with ceiling lower than A's */
    debug_println("y: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("y: should get signal last, taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M2);
    debug_println("y: sending signal to b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("y: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("y: still running at greater priority than b. releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M2);

    /* Part 3: Two mutexes with differing ceilings */
    debug_println("y: taking lock H (> b)");
    rtos_mutex_lock(RTOS_MUTEX_ID_M3_H);
    debug_println("y: sending signal to b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("y: should still run (> b), sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("y: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("y: got signal, releasing lock H");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M3_H);
    debug_println("y: sending signal to b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P3);

    /* Part 4: B's lock attempt times out */
    debug_println("y: sleeping");
    rtos_sleep(PART_4_SLEEP);

    /* Part 5: B gets lock before timeout */
    debug_println("y: sleeping");
    rtos_sleep(PART_5_SLEEP);

    debug_println("y: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_z(void)
{
    /* Part 1: Mutex with ceiling higher than all tasks */
    debug_println("z: taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("z: sending signal to y");
    rtos_signal_send_set(RTOS_TASK_ID_Y, RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("z: sending signal to b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("z: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P1);
    debug_println("z: still running at greater priority than a, b and y. releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);

    /* Part 2: Mutex with ceiling lower than A's */
    debug_println("z: taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M2);
    debug_println("z: sending signal to y");
    rtos_signal_send_set(RTOS_TASK_ID_Y, RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("z: sending signal to b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("z: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P2);
    debug_println("z: still running at greater priority than b and y. releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M2);

    /* Part 3: Two mutexes with differing ceilings */
    debug_println("z: taking lock L (> y)");
    rtos_mutex_lock(RTOS_MUTEX_ID_M3_L);
    debug_println("z: sending signal to y");
    rtos_signal_send_set(RTOS_TASK_ID_Y, RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("z: should run (> y), sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("z: sending signal to b");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_DEMO_P3);
    debug_println("z: releasing lock L");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M3_L);

    /* Part 4: B's lock attempt times out */
    debug_println("z: sleeping");
    rtos_sleep(PART_4_SLEEP);

    /* Part 5: B gets lock before timeout */
    debug_println("z: sleeping");
    rtos_sleep(PART_5_SLEEP);

    debug_println("z: shouldn't be here!");
    for (;;)
    {
    }
}

int
main(void)
{
    machine_timer_start((void (*)(void))tick_irq);

    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
