#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "os_task.h"

extern const task_t app_task_table[];

/*****************************************************************************/
/*  DECLARE: Task ID
 *  
 */
/*****************************************************************************/
enum
{
	/* SYSTEM TASKS */

	/* APP TASKS */
	TASK_1_ID,
	TASK_2_ID,
	TASK_3_ID,

	/* EOT task ID (Size of task table)*/
	TASK_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Task function
 */
/*****************************************************************************/
/* APP TASKS */
extern void task_1(void *p_arg);
extern void task_2(void *p_arg);
extern void task_3(void *p_arg);
#endif //__TASK_LIST_H__
