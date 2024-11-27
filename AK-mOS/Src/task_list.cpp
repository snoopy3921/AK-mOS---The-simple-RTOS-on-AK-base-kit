#include "task_list.h"

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
