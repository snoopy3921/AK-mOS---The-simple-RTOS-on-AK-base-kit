# AK-mOS - The simple RTOS on AK base kit
![ak-embedded-software-logo](https://github.com/user-attachments/assets/950faf1e-97a8-4c08-8ec5-7a07cee47c2b)



AK-mOS is a mini embedded operating system developed based on [freeRTOS](https://github.com/FreeRTOS/FreeRTOS-Kernel) which has the following features:
- Preemptive scheduling
- Round-robin scheduling
- Inner tasks communiation
- Software timer

## Port
Kernel required tick interrupt and context switch (PendSV interrupt) to work properly. Both tick interrupt and context switch written for ARM Cortex-M3 only ([AK base kit](https://github.com/epcbtech/ak-base-kit-stm32l151) using Stm32L1). So it will also run fine on Stm32f1.

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
APIs:

- Delay an amount of ticks (normally 1 tick = 1ms), pass OS_CFG_DELAY_MAX to block indefinitely till the task get an unblock event.
``` C
void os_task_delay(const uint32_t tick_to_delay);
```

### 3. Memory allocation and dealocation
Using first-fit allocation that make the use of memory simple, effective and minimize memory fragmentaion, but it costs disadvantages, mainly on performance if the frequency alloc and free was pretty high.

APIs:

``` C
   void *os_mem_malloc(size_t size);	// In bytes

   void os_mem_free(void *p_addr);
```
These APIs are internally used in kernel to manage memmory of task and messages, but can also use in applcation if needed, of instead using APIs from "stdlib.h" (malloc and free)
### 3. Communication (messages)
Kernel has one pool to store free messages. Firstly all the messages is kept in message pool. There are 2 types of msg:
- Pure msg contains only signal type int16_t
- Dynamic msg contains pointer to data block and size of that data block
APIs:
``` C
  void os_task_post_msg_dynamic(uint8_t des_task_id, void *p_content, uint8_t msg_size);

  void os_task_post_msg_pure(uint8_t des_task_id, int32_t sig);

  msg_t *os_task_wait_for_msg(uint32_t time_out);
```
**Recommendation using communicated APIs:**

Post msg to another task, kernel doesn't support to post msg to self
- Pure msg post
``` C
  #define AC_DISPLAY_BUTTON_MODE_PRESSED (1u)

  os_task_post_msg_pure (TASK_BUZZER_ID, AC_DISPLAY_BUTTON_MODE_PRESSED);
```
- Dynamic msg post
``` C
  time[3] = {23, 15, 30}; //hour, min, second

  os_task_post_msg_dynamic (TASK_DISPLAY_ID, (void *) &time, sizeof(time));
```

Task can wait for msg with timeout or indefinitely (as it delays indefinitely). Retrieving msg from "os_task_wait_for_msg", msg could be NULL (timeout expired) or success.
After consuming msg. If you don't have intention to use it after. It is required to free msg to give msg back to msg pool and give memmory back to kernel (In case using dynamic msg).
Call free msg with this API:
``` C
  void os_msg_free(msg_t *p_msg);
```
A task consumes msg looks like this:
- Task wait for msg indefinitely
``` C
void task_buzzer(void *p_arg)
{
	int play = 0;
	msg_t * msg; 
	for(;;)
	{			
		msg = os_task_wait_for_msg (OS_CFG_DELAY_MAX);
		play = msg->sig;
		os_msg_free(msg);
		if(play)
		{
			/*Play tone*/
			play = 0;
		}
	
	}
}
```
- Task wait for msg with timeout
``` C
void task_display(void *p_arg)
{
	msg_t * msg;
	for(;;)
	{		
		msg = os_task_wait_for_msg(1); // Wait for 1ms
		if(msg!= NULL)
		{
			if(msg->sig == 1)
			{
				//SYS_PRINT("BUTTON UP RECEIVED\n");
			}
			else
			{
				//SYS_PRINT("BUTTON DOWN RECEIVED\n");
			}
			os_msg_free(msg);
		}
		view_render.update();
	}
}
```
### 4. Ending up in main file
``` C
int main(void)
{
	//Init system
	//Init drivers
	
	os_init();
	
	os_task_create_list((task_t*)app_task_table, TASK_EOT_ID);
	
	os_run();
	
	
	while(1)
	{	
		//Hopefully this will never run )))))
	}
}
```
### 5. Additional
Using interrupt by "deferred interrupt handling". It is better to create a interrupt task with a high enough priority that wait for signal (msg) from interrupt.
``` C
#ifdef __cplusplus
extern "C"
{
#endif
void EXTI0_IRQHandler(void)
{
	ENTER_CRITICAL();
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) 
	{		
		os_task_post_msg_pure (TASK_BUTTON_INTERRUPT_ID, INTERRUPT_TRIGGER_SIGNAL);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
	EXIT_CRITICAL();
}
#ifdef __cplusplus
}
#endif
```
**NOTE: If application is C++ function names are mangled by the compiler, so the linker cannot match the name in the vector table to the user-written ISR and falls back to the default "weak" handler, usually implemented as an empty infinite loop. The canonical way to avoid name mangling in C++ is to enclose the given function (ISR) into extern "C"{} block.**
## Notes
``` C
SYS_PRINT("	THE END!	");
```
## TODO:
Add description about software timer.
