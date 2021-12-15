#include "../../FX-RTOS/FXRTOS.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
// #include "FXRTOS.h"
#include <stdio.h>
#include <string.h>


fx_dpc_t dpc;
fx_sem_t semaphore;


extern void led_on(void);
extern void led_off(void);
extern TIM_HandleTypeDef htim3;

void
task(void* args)
{
    for (;;)
    {
    	printf("Going to wait for interrupt\n\r");
        fx_sem_wait(&semaphore, NULL);
        printf("Semaphore opened\n\r");
        led_on();
        fx_thread_sleep(100);
        led_off();
    }
    fx_thread_exit();
}

void
tim3_dpc(fx_dpc_t* dpc, void* args)
{
    fx_sem_post(&semaphore);
}

void
tim3_handler(void)
{
	__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
	fx_dpc_request(&dpc, tim3_dpc, NULL);
}

void
fx_app_init(void)
{
    // Creating thread and its stack.
    static fx_thread_t thread;
    static uint32_t stack[0x200 / sizeof(uint32_t)];

    // Initializing thread, semaphore and dpc.
    fx_dpc_init(&dpc);
    fx_sem_init(&semaphore, 0, 1, FX_SYNC_POLICY_FIFO);
    fx_thread_init(&thread, task, NULL, 10, stack, sizeof(stack), 0);
}

void
fx_intr_handler(void)
{
    switch (hal_intr_get_current_vect())
    {
        case TIM3_IRQn:
            tim3_handler();
            break;
        default:
            break;
    }
}
