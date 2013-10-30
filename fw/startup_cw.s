/*****************************************************************************
 * Copyright (c) 2010 Rowley Associates Limited.                             *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *****************************************************************************/

/*****************************************************************************
 *                           Preprocessor Definitions
 *                           ------------------------
 *
 * STARTUP_FROM_RESET
 *
 *   If defined, the program will startup from power-on/reset. If not defined
 *   the program will just loop endlessly from power-on/reset.
 *
 *   This definition is not defined by default on this target because the
 *   debugger is unable to reset this target and maintain control of it over the
 *   JTAG interface. The advantage of doing this is that it allows the debugger
 *   to reset the CPU and run programs from a known reset CPU state on each run.
 *   It also acts as a safety net if you accidently download a program in FLASH
 *   that crashes and prevents the debugger from taking control over JTAG
 *   rendering the target unusable over JTAG. The obvious disadvantage of doing
 *   this is that your application will not startup without the debugger.
 *
 *   We advise that on this target you keep STARTUP_FROM_RESET undefined whilst
 *   you are developing and only define STARTUP_FROM_RESET when development is
 *   complete.
 *
 * NO_SYSTEM_INIT
 *
 *   If defined, the SystemInit() function will NOT be called. By default 
 *   SystemInit() is called after reset to enable the clocks and memories to
 *   be initialised prior to any C startup initialisation.
 *
 *****************************************************************************/

#if defined (__CODE_RED)
#else

  .global reset_handler
  .syntax unified

  #include <LPC1000.h>

  .section .vectors, "ax"
  .code 16
  .align 0
  .global _vectors

.macro DEFAULT_ISR_HANDLER name=
  .thumb_func
  .weak \name
\name:
1: b 1b /* endless loop */
.endm

_vectors:
  .word __stack_end__
#ifdef STARTUP_FROM_RESET
  .word reset_handler
#else
  .word reset_wait
#endif /* STARTUP_FROM_RESET */
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0 // Reserved
  .word 0 // Reserved
  .word 0 // Reserved
  .word 0 // Reserved
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0 // Reserved
  .word PendSV_Handler
  .word SysTick_Handler 
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word WAKEUP_IRQHandler
  .word CAN_IRQHandler
  .word SSP1_IRQHandler
  .word I2C_IRQHandler
  .word TIMER16_0_IRQHandler
  .word TIMER16_1_IRQHandler
  .word TIMER32_0_IRQHandler
  .word TIMER32_1_IRQHandler
  .word SSP0_IRQHandler
  .word UART_IRQHandler
  .word 0 // Reserved
  .word 0 // Reserved
  .word ADC_IRQHandler
  .word WDT_IRQHandler
  .word BOD_IRQHandler
  .word 0 // Reserved
  .word PIOINT3_IRQHandler
  .word PIOINT2_IRQHandler
  .word PIOINT1_IRQHandler
  .word PIOINT0_IRQHandler 

  .section .init, "ax"
  .thumb_func

reset_handler:
  ldr r0, =SYSCON_BASE_ADDRESS
#ifdef __FLASH_BUILD
  ldr r1, =2
#else
  ldr r1, =1
#endif
  str r1, [r0, #SYSMEMREMAP_OFFSET]

#ifndef NO_SYSTEM_INIT
  ldr r0, =__RAM_segment_end__
  mov sp, r0
  ldr r0, =SystemInit
  blx r0
#endif

  b _start

DEFAULT_ISR_HANDLER NMI_Handler
DEFAULT_ISR_HANDLER HardFault_Handler
DEFAULT_ISR_HANDLER MemManage_Handler
DEFAULT_ISR_HANDLER BusFault_Handler
DEFAULT_ISR_HANDLER UsageFault_Handler
DEFAULT_ISR_HANDLER SVC_Handler 
DEFAULT_ISR_HANDLER DebugMon_Handler
DEFAULT_ISR_HANDLER PendSV_Handler
DEFAULT_ISR_HANDLER SysTick_Handler 
DEFAULT_ISR_HANDLER WAKEUP_IRQHandler
DEFAULT_ISR_HANDLER CAN_IRQHandler
DEFAULT_ISR_HANDLER SSP1_IRQHandler
DEFAULT_ISR_HANDLER I2C_IRQHandler
DEFAULT_ISR_HANDLER TIMER16_0_IRQHandler
DEFAULT_ISR_HANDLER TIMER16_1_IRQHandler
DEFAULT_ISR_HANDLER TIMER32_0_IRQHandler
DEFAULT_ISR_HANDLER TIMER32_1_IRQHandler
DEFAULT_ISR_HANDLER SSP0_IRQHandler
DEFAULT_ISR_HANDLER UART_IRQHandler
DEFAULT_ISR_HANDLER ADC_IRQHandler
DEFAULT_ISR_HANDLER WDT_IRQHandler
DEFAULT_ISR_HANDLER BOD_IRQHandler
DEFAULT_ISR_HANDLER PIOINT3_IRQHandler
DEFAULT_ISR_HANDLER PIOINT2_IRQHandler
DEFAULT_ISR_HANDLER PIOINT1_IRQHandler
DEFAULT_ISR_HANDLER PIOINT0_IRQHandler

#ifndef STARTUP_FROM_RESET
DEFAULT_ISR_HANDLER reset_wait
#endif /* STARTUP_FROM_RESET */

#endif