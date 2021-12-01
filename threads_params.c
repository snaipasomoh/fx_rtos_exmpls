// Reusing thread's struct and stack. Getting and setting thread's params.
// demonstration of deinit, get_params, set_params
#include "../../FX-RTOS/FXRTOS.h"
// #include "FXRTOS.h"
#include "stdio.h"
#include "string.h"

extern void led_on(void);
extern void led_off(void);

void
task_0(void *args)
{
    unsigned selfPrio;
    // Pointer to thread can be obtained from it's task with thread_self.
    // Some thread params can be obtained with thread_get_params.
    fx_thread_get_params(fx_thread_self(), FX_THREAD_PARAM_PRIO, &selfPrio);
    printf("Hello from task_0 with prio = %u\n\r", selfPrio);
    fx_thread_exit();
}

void
task_1(void *args)
{
    unsigned selfPrio;
    fx_thread_get_params(fx_thread_self(), FX_THREAD_PARAM_PRIO, &selfPrio);
    printf("Hello from task_1 with prio = %u\n\r", selfPrio);
    fx_thread_exit();
}

void
task_main(void *args)
{
    // Creating thread and it's stack.
    static fx_thread_t thread;
    static uint32_t stack[0x200 / sizeof(uint32_t)];
    
    // Initializing thread with task_0 and priority level 10.
    fx_thread_init(&thread, task_0, NULL, 10, stack, sizeof(stack), false);
    // Waiting for thread's task ending.
    fx_thread_join(&thread);

    // Thread structure and it's stack can be reused but deinit must be called
    // before.
    fx_thread_deinit(&thread);
    // Reinitializing thread with other task and priority level.
    fx_thread_init(&thread, task_1, NULL, 9, stack, sizeof(stack), false);
    fx_thread_join(&thread);

    fx_thread_deinit(&thread);
    fx_thread_init(&thread, task_0, NULL, 10, stack, sizeof(stack), false);
    // Priority level can be changed with set_params after initializing thread.
    fx_thread_set_params(&thread, FX_THREAD_PARAM_PRIO, 7);
    fx_thread_join(&thread);

    fx_thread_exit();
}

void
fx_app_init(void)
{
    static fx_thread_t thread_main;
    static uint32_t stack_main[0x400 / sizeof(uint32_t)];
    fx_thread_init(&thread_main, task_main, NULL, 6, stack_main, 
                   sizeof(stack_main), false);
}

void
fx_intr_handler(void)
{
    ;
}
