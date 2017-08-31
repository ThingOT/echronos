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
/*<module>
    <code_gen>template</code_gen>
    <headers>
        <header path="../../rtos-example/machine-timer.h" code_gen="template" />
    </headers>
</module>*/

#include "machine-timer.h"

static void decrementer_clear(void);

void
machine_timer_start(__attribute__((unused)) void (*application_timer_isr)(void))
{
    /* U-Boot has the decrementer on, so we turn this off here */
    machine_timer_stop();

    /*
     * Configure a fixed interval timer
     *
     * Enable:
     *  In TCR (timer control register)
     *    TCR[FIE] (fixed-interval interrupt enable)
     *
     * Set period: TCR[FPEXT] || TCR[FP]
     */
    asm volatile(
        /*
         * 1 TB (time base) period occurs every 8 CCB periods (by default setting of HID0[SEL_TBCLK]=0).
         *
         * On P2020, CCB (Core Complex Bus) clock frequency ranges from 800MHz to 1.3GHz.
         * 0x2810000 => 1000_10: TBL[33]: 30th bit from lsb: 2^30 = 1,073,741,824 TB periods
         *      At 800MHz this is ~11s, and at 1.3GHz this is ~7s.
         * 0x3810000 => 1000_11: TBL[34]: 29th bit from lsb: 2^29 ~ 3-5s
         * 0x3812000 => 1001_11: TBL[38]: about 1/3 of a second
         */
        "mftcr %%r3\n"
        "oris %%r3,%%r3,0x381\n"
        "ori %%r3,%%r3,0x2000\n"
        "mttcr %%r3"
        ::: "r3");
}

/* Useful primarily in case of bootloaders that make use of timer interrupts */
void
machine_timer_stop(void)
{
    asm volatile(
        "li %%r3,0\n"
        "mttcr %%r3"
        ::: "r3");

    /* In case there are any decrementer interrupts pending, clear them */
    decrementer_clear();
}

static void
decrementer_clear(void)
{
    asm volatile(
        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[DIS] (decrementer interrupt status) */
        "lis %%r3,0x800\n"
        "mttsr %%r3\n"
        ::: "r3");
}

void
machine_timer_tick_isr(void)
{
    asm volatile(
        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[FIS] (fixed-interval timer interrupt status) */
        "lis %%r3,0x400\n"
        "mttsr %%r3\n"
        ::: "r3");
}
