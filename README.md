# AK-mOS - The simple RTOS on AK base kit
 AK-mOS is a mini embedded operating system developed based on theory freeRTOS which has the following features:
- Preemptive scheduling
- Round-robin scheduling
- Inner tasks communiation
## Port
Kernel required tick interrupt and context switch (PendSV interrupt) to work properly. Both tick interrupt and context switch written for ARM Cortex-M3 only (AK base kit using Stm32L1). So it will also run fine on Stm32f1.

In file os_cpu.h, change these header files to appropriate microcontroller (ARM-Cortex M3)
``` C
#include "stm32l1xx.h"
#include "core_cm3.h"
#include "core_cmFunc.h"
```
For porting in the future, different "os_cpu" files are needed for different architectures.

## Getting started

Add the kernel to project, and in application implement file (normally main.c or main.cpp), include these header files.
``` C
#include "os_kernel.h"  
#include "os_mem.h"     
#include "os_task.h"   
#include "os_msg.h"
#include "task_list.h"
```
### 1. Configuration
All config needed are included in os_cfg.h file with the simplicity of this OS, these are some important explanations:

The memory management first acquires this amount of memory (In byte) and use it to alloc to which component need memory also retrieve memory back.
``` C
#define OS_CFG_HEAP_SIZE                  ((size_t)1024 * 3u)
```

Tasks can have priority from 0 to OS_CFG_PRIO_MAX - 1
``` C
#define OS_CFG_PRIO_MAX                   (30u)
```

Minimum stack size for tasks to run well.
``` C
#define OS_CFG_TASK_STK_SIZE_MIN          ((size_t)32u) // In stack, equal to x 4 bytes
```

Kernel has a pool to store messages, increase OS_CFG_MSG_POOL_SIZE if application use large amount of messages.

``` C
#define OS_CFG_MSG_POOL_SIZE              (16u)
```
### 2. Task creation and using
Tasks in AK-mOS are pre-created (because of lacking scheduler locker), so that to create tasks, require to create before kernel running.

To create tasks, register task parameters in "task_list.h" and ""task_list.cpp":

In task_list.h, declare task ID in enum, and task funtions. Declare taskID in enum in increasing order start with unsigned integer is important because this help kernel to manage tasks in task list array.
At the end of task ID, don't change or remove TASK_EOT_ID, it informs kernel about number of tasks.
``` C
/*****************************************************************************/
/*  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 */
/*****************************************************************************/
enum
{
	/* SYSTEM TASKS */

	/* APP TASKS */
	TASK_BUTTONS_ID,
	TASK_2_ID,
	TASK_DISPLAY_ID,
	TASK_BUZZER_ID,

	/* EOT task ID (Size of task table)*/
	TASK_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Task function
 */
/*****************************************************************************/
/* APP TASKS */
extern void task_buttons(void *p_arg);
extern void task_2(void *p_arg);
extern void task_display(void *p_arg);
extern void task_buzzer(void *p_arg);
```

In task_list.cpp, put parameters for each task in this order (task id, task_func, arg, prio, msg_queue_size, stk_size)
- task id pick from enum task id in task_list.h
- task_func also pick from task funtion from task_list.h
- arg is the argument pass to funtions (not tested yet).
- priority of task, the lower, the more important. 0 is the highest priority.
- msg_queue_size is the size of queue message in task. For tasks that doesn't need to receive message(signal or data), just leave it zero.
- stk_size is the size allocated for each task. Minimum stack size declared in os_cfg.h.
  - [**NOTE: With heavy task, increase it. If program doesn't run, increase it!!!**](https://stackoverflow.com/)

``` C
const task_t app_task_table[] = {
    /*************************************************************************/
    /* TASK */
    /* TASK_ID          task_func       arg     prio     msg_queue_size                     stk_size */
    /*************************************************************************/
    {TASK_2_ID,         task_2,         NULL,   10,     OS_CFG_TASK_MSG_Q_SIZE_NORMAL,    32},
    {TASK_BUTTONS_ID,   task_buttons,   NULL,   0,      OS_CFG_TASK_MSG_Q_SIZE_NORMAL,    50},
    {TASK_DISPLAY_ID,   task_display,   NULL,   8,      OS_CFG_TASK_MSG_Q_SIZE_NORMAL,    200},
    {TASK_BUZZER_ID,    task_buzzer,    NULL,   5,      OS_CFG_TASK_MSG_Q_SIZE_NORMAL,    50},

};
```
A task looks like this:
``` C
void task_2(void *p_arg)
{
	for(;;)
	{		
		os_task_delay(1000);
	}
}
```
### 2. Memory allocation and dealocation
Using first-fit allocation that make the use of memory simple, effective and minimize memory fragmentaion, but it costs disadvantages, mainly on performance if the frequency alloc and free was pretty high.

APIs:

``` C
   void *os_mem_malloc(size_t size);
   void os_mem_free(void *p_addr);
```
These APIs are internally used in kernel to manage memmory of task and messages, but can also use in applcation if needed, of instead using APIs from "stdlib.h" (malloc and free)
