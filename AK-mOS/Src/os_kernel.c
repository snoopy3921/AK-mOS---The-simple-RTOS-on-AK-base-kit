#include "os_kernel.h"

#include "os_cpu.h"
#include "os_msg.h"
#include "os_timer.h"
#include "os_prio.h"
#include "os_task.h"
#include "os_log.h"
#include "task_list.h"

static uint16_t critical_nesting_count = (uint16_t)0u;

void os_critical_enter(void)
{
    DISABLE_INTERRUPTS
    critical_nesting_count++;
}

void os_critical_exit(void)
{
    os_assert(critical_nesting_count, "NESTING CRITICAL UNBALANCED");
    critical_nesting_count--;
    if (critical_nesting_count == 0)
    {
        ENABLE_INTERRUPTS
    }
}

static void
os_start_first_task(void)
{
    __asm volatile(
        " ldr r0, =0xE000ED08 	    \n" /* Use the NVIC offset register to locate the stack. */
        " ldr r0, [r0] 	            \n"
        " ldr r0, [r0] 			    \n"
        " msr msp, r0			    \n" /* Set the msp back to the start of the stack. */
        " cpsie i			        \n" /* Globally enable interrupts. */
        " cpsie f		            \n"
        " dsb			            \n"
        " isb			            \n"
        " svc 0		                \n" /* System call to start first task. */
        " nop			            \n"
        " .ltorg                    \n"
    );
}

void os_init(void)
{
    if (TASK_EOT_ID < 1u)
    {
        LOG_ERROR("OS_ERR_NO_TASK_AVAILABLE - Entering while loop");
        DISABLE_INTERRUPTS
        while(1);
    }
    os_prio_init();
    os_msg_init();
    os_timer_init();
}

void os_run(void)
{
    os_task_start();
    os_cpu_systick_init_freq(SystemCoreClock);
    os_cpu_setup_PendSV();
    os_start_first_task();
}
