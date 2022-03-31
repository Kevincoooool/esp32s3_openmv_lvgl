/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "pendsv.h"
#include "RP2040.h"

// This variable is used to save the exception object between a ctrl-C and the
// PENDSV call that actually raises the exception.  It must be non-static
// otherwise gcc-5 optimises it away.  It can point to the heap but is not
// traced by GC.  This is okay because we only ever set it to
// mp_kbd_exception which is in the root-pointer set.
void *pendsv_object;

#if defined(PENDSV_DISPATCH_NUM_SLOTS)
uint32_t pendsv_dispatch_active;
pendsv_dispatch_t pendsv_dispatch_table[PENDSV_DISPATCH_NUM_SLOTS];
#endif

void pendsv_init(void) {
    pendsv_object = NULL;
    MP_STATE_MAIN_THREAD(mp_pending_exception) = MP_OBJ_NULL;
    #if defined(PENDSV_DISPATCH_NUM_SLOTS)
    pendsv_dispatch_active = false;
    #endif
    NVIC_DisableIRQ(PendSV_IRQn);
    __ISB();
    // Set PendSV to lowest priority.
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(2, 3, 0));
    __ISB();
    NVIC_EnableIRQ(PendSV_IRQn);
    __ISB();
}

// Call this function to raise a pending exception during an interrupt.
// It will first try to raise the exception "softly" by setting the
// mp_pending_exception variable and hoping that the VM will notice it.
// If this function is called a second time (ie with the mp_pending_exception
// variable already set) then it will force the exception by using the hardware
// PENDSV feature.  This will wait until all interrupts are finished then raise
// the given exception object using nlr_jump in the context of the top-level
// thread.
void pendsv_kbd_intr(void) {
    if (MP_STATE_MAIN_THREAD(mp_pending_exception) == MP_OBJ_NULL) {
        mp_sched_keyboard_interrupt();
    } else {
        MP_STATE_MAIN_THREAD(mp_pending_exception) = MP_OBJ_NULL;
        pendsv_object = &MP_STATE_VM(mp_kbd_exception);
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    }
}

// This will always force the exception by using the hardware PENDSV 
void pendsv_nlr_jump(void *o) {
    MP_STATE_MAIN_THREAD(mp_pending_exception) = MP_OBJ_NULL;
    pendsv_dispatch_active = false;
    pendsv_object = o;
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __ISB();
}

#if defined(PENDSV_DISPATCH_NUM_SLOTS)
void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f) {
    pendsv_dispatch_table[slot] = f;
    pendsv_dispatch_active = true;
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __ISB();
}

void pendsv_dispatch_handler(void) {
    for (size_t i = 0; i < PENDSV_DISPATCH_NUM_SLOTS; ++i) {
        if (pendsv_dispatch_table[i] != NULL) {
            pendsv_dispatch_t f = pendsv_dispatch_table[i];
            pendsv_dispatch_table[i] = NULL;
            f();
        }
    }
}
#endif

__attribute__((naked)) void PendSV_Handler(void) {
    // Handle a PendSV interrupt
    //
    // For the case of an asynchronous exception, re-jig the
    // stack so that when we return from this interrupt handler
    // it returns instead to nlr_jump with argument pendsv_object
    //
    // on entry to this (naked) function, stack has the following layout:
    //   sp[6]: pc=r15
    //   sp[5]: lr=r14
    //   sp[4]: r12
    //   sp[3]: r3
    //   sp[2]: r2
    //   sp[1]: r1
    //   sp[0]: r0

    __asm volatile (
        #if defined(PENDSV_DISPATCH_NUM_SLOTS)
        // Check if there are any pending calls to dispatch to
        "ldr r1, pendsv_dispatch_active_ptr\n"
        "ldr r0, [r1]\n"
        "cmp r0, #0\n"
        "beq .no_dispatch\n"
        "mov r2, #0\n"
        "str r2, [r1]\n"                // clear pendsv_dispatch_active
        "b pendsv_dispatch_handler\n"   // jump to the handler
        ".no_dispatch:\n"
        #endif

        // Check if there is an active object to throw via nlr_jump
        "ldr r1, pendsv_object_ptr\n"
        "ldr r0, [r1]\n"
        "cmp r0, #0\n"
        "beq .no_obj\n"

        "mov r2,  #1 \n"                // Modify stacked XPSR to make sure
        "lsl r2,  #24\n"                // possible LDM/STM progress is cleared.
        "str r2, [sp, #28] \n"          

        "str r0, [sp, #0]\n"            // store to r0 on stack
        "mov r0, #0\n"
        "str r0, [r1]\n"                // clear pendsv_object
        "ldr r0, nlr_jump_ptr\n"
        "str r0, [sp, #24]\n"           // store to pc on stack
        "bx lr\n"                       // return from interrupt; will return to nlr_jump
        ".no_obj:\n"                    // pendsv_object==NULL

        // Spurious pendsv, just return
        "bx lr\n"

        // Data
        ".align 2\n"
        #if defined(PENDSV_DISPATCH_NUM_SLOTS)
        "pendsv_dispatch_active_ptr: .word pendsv_dispatch_active\n"
        #endif
        "pendsv_object_ptr: .word pendsv_object\n"
        "nlr_jump_ptr: .word nlr_jump\n"
        );
}
