#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "os_task.h"

extern const task_t app_task_table[];

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

#endif //__TASK_LIST_H__
