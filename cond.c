#include "../../FX-RTOS/FXRTOS.h"
// #include "FXRTOS.h"
#include "stdio.h"
#include "string.h"

fx_cond_t cond_0;
fx_cond_t cond_1;
fx_cond_t cond_2;

fx_mutex_t mutex_0;
fx_mutex_t mutex_1;
fx_mutex_t mutex_2;

extern void led_on(void);
extern void led_off(void);

// This task is used by 3 threads. Here each thread stops on 3 condvars.
// Task_main opens all this condvars.
void
task(void* args)
{
    printf("Thread_%c started\n\r", *(char*)args);
    fx_mutex_acquire(&mutex_0, NULL);
    fx_cond_wait(&cond_0, &mutex_0, NULL);
    fx_mutex_release(&mutex_0);

    printf("Thread_%c passed cond_0\n\r", *(char*)args);
    fx_mutex_acquire(&mutex_1, NULL);
    fx_cond_wait(&cond_1, &mutex_1, NULL);
    fx_mutex_release(&mutex_1);

    printf("Thread_%c passed cond_1\n\r", *(char*)args);
    fx_mutex_acquire(&mutex_2, NULL);
    fx_cond_wait(&cond_2, &mutex_2, NULL);
    fx_mutex_release(&mutex_2);

    printf("Thread_%c passed cond_2\n\r", *(char*)args);
    fx_thread_exit();
}

// There is condvar control in task_main.
void
task_main(void* args)
{
    printf("Task_main started\n\r");
    printf("\nPass through cond_0 one by one\n\r");
    fx_thread_sleep(5000);
    for (unsigned i = 0; i < 3; i++){
        fx_cond_signal(&cond_0);
        fx_thread_sleep(2000);
    }
    fx_thread_sleep(3000);

    printf("\nPass through cond_1 all together\n\r");
    fx_cond_broadcast(&cond_1);
    fx_thread_sleep(5000);

    printf("\nPass through cond_2 all together with deiniting it\n\r");
    fx_cond_deinit(&cond_2);
    fx_thread_exit();
}


void
fx_app_init(void)
{
    // Creating threads and their stacks.
    static fx_thread_t thread_0;
    static fx_thread_t thread_1;
    static fx_thread_t thread_2;
    static fx_thread_t thread_main;
    static uint32_t stack_0[0x200 / sizeof(uint32_t)];
    static uint32_t stack_1[0x200 / sizeof(uint32_t)];
    static uint32_t stack_2[0x200 / sizeof(uint32_t)];
    static uint32_t stack_main[0x200 / sizeof(uint32_t)];

    // Initializing threads, conds and mutexes.
    fx_cond_init(&cond_0, FX_SYNC_POLICY_DEFAULT);
    fx_cond_init(&cond_1, FX_SYNC_POLICY_DEFAULT);
    fx_cond_init(&cond_2, FX_SYNC_POLICY_DEFAULT);
    fx_mutex_init(&mutex_0, FX_MUTEX_CEILING_DISABLED, FX_SYNC_POLICY_DEFAULT);
    fx_mutex_init(&mutex_1, FX_MUTEX_CEILING_DISABLED, FX_SYNC_POLICY_DEFAULT);
    fx_mutex_init(&mutex_2, FX_MUTEX_CEILING_DISABLED, FX_SYNC_POLICY_DEFAULT);
    fx_thread_init(&thread_0, task, "0", 10, stack_0, sizeof(stack_0), 0);
    fx_thread_init(&thread_1, task, "1", 10, stack_1, sizeof(stack_1), 0);
    fx_thread_init(&thread_2, task, "2", 10, stack_2, sizeof(stack_2), 0);
    fx_thread_init(&thread_main, task_main, NULL, 10, stack_main,
                   sizeof(stack_main), 0);
}

void
fx_intr_handler(void)
{
    ;
}
