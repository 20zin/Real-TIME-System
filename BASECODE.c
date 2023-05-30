#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>
#include <conio.h>
#include  <time.h>
#include  "app_cfg.h"

#define         OS_TIME_DELAY
#define         LOOP         250

#define         TASK_STK_SIZE    256      /* Size of each task's stacks (# of OS_STK)      */
#define         MSG_QUEUE_SIZE   20

OS_STK          TaskStartStk[TASK_STK_SIZE];   /*½ºÅÃ¿µ¿ª*/
OS_STK          Task1Stk[TASK_STK_SIZE];       /* Task #1    task stack*/
