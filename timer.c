#include "timer.h"
#include "stm32f0xx.h"

#ifndef NULL
#define NULL ((void *)0x00)
#endif

#ifndef TMR_AMOUNT
#define TMR_AMOUNT    20
#endif

typedef struct timer
{
    timer_callback_t cbk;
    timer_type_t type;
    uint32_t period;
    uint32_t timeout;
    timer_state_t state;
}
timer_t;

static timer_t timers[TMR_AMOUNT] = {{NULL, TYPE_NOT_DEFINED, 0, 0, TIMER_EMPTY}};
static volatile uint32_t systemtick = 0;
static volatile uint32_t delay_time = 0;
static uint8_t timer_is_initialized = 0;

void SysTick_Handler(void)
{
	systemtick++;
	delay_time++;
}

/**
 * @brief Ativa a PLL como clock do sistema, multiplicando o HSI (8 MHz) por
 * 6, resultando em 48 MHz de clock no sistema.
 *
 */
static void Timer_Activate_PLL_System_Clock(void)
{
    // ativa o HSI como clock do sistema e aguarda ate a mudanca ser feita
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)
    {
        RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW);
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
    }

    // desliga a PLL para configura-la
    RCC->CR &= (uint32_t)(~RCC_CR_PLLON);
    while((RCC->CR & RCC_CR_PLLRDY) != 0);

    // configura a PLL com multiplicador de 6x, para gerar 48 MHz
    RCC->CFGR &= ~(RCC_CFGR_PLLMUL);
    RCC->CFGR |= RCC_CFGR_PLLMUL6;

    // ativa a PLL
    RCC->CR |= RCC_CR_PLLON;
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    // seta a PLL como clock do sistema e aguarda ate a mudanca ser feita
    RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void Timer_Init(void)
{
    // ativa PLL como clock do sistema - 48MHz
	Timer_Activate_PLL_System_Clock();

    // atualiza a variavel SystemCoreClock
    SystemCoreClockUpdate();

	SysTick_Config(SystemCoreClock / TIMEBASE);

	timer_is_initialized = 1;

    for (uint16_t i = 0; i < TMR_AMOUNT; i++)
    {
        timers[i].state = TIMER_EMPTY;
        timers[i].type = TYPE_NOT_DEFINED;
    }

	NVIC_SetPriority(SysTick_IRQn, 1);

}

/**
 * @brief Get current System tick value.
 *
 * @return uint32_t System tick.
 */
uint32_t Timer_GetSystemTick(void)
{
	return systemtick;
}

/**
 * @brief Timer State Machine
 *
 */
void Timer_SM(void)
{
    static uint16_t timer_cnt = 0;
    uint32_t tick = systemtick;

    if (0 == timer_is_initialized)
    {
        return;
    }

    if ((TIMER_RUNNING == timers[timer_cnt].state) && \
        (systemtick >= timers[timer_cnt].timeout))
    {
        if (NULL != timers[timer_cnt].cbk)
        {
            timers[timer_cnt].cbk((void*)&tick, 1);
        }
        else
        {
            timers[timer_cnt].state = TIMER_ERROR;
        }

        switch (timers[timer_cnt].type)
        {
            case ONE_SHOT_TIMER:
                timers[timer_cnt].state = TIMER_STOPPED;
            break;

            case AUTO_RELOAD_TIMER:
                timers[timer_cnt].timeout += timers[timer_cnt].period;
            break;

            default:
                timers[timer_cnt].state = TIMER_ERROR;
            break;
        }

    }

    if (TMR_AMOUNT <= (++timer_cnt))
    {
        timer_cnt = 0;
    }

}

/**
 * @brief Creates a timer.
 *
 * Be aware that this is a blocking function!!!
 *
 * @param timer_cbk Timer callback.
 * @param timer_type Timer type.
 * @param timer_period Timer period. Use the Defines TIME_x
 * @return uint16_t Returns timer ID or 0xFFFF if cannot creates it.
 */
uint16_t Timer_Create(timer_callback_t timer_cbk, timer_type_t timer_type, \
    uint32_t timer_period)
{
    uint16_t result = UINT16_MAX;

    for (uint16_t i = 0; i < TMR_AMOUNT; i++)
    {
        if (TIMER_EMPTY == timers[i].state)
        {
            if (NULL == timer_cbk)
            {
                /* do nothing */
            }
            else if ((AUTO_RELOAD_TIMER < timer_type) || (0 == timer_period))
            {
                /* do nothing */
            }
            else
            {
                timers[i].cbk = timer_cbk;
                timers[i].type = timer_type;
                timers[i].period = timer_period;
                timers[i].timeout = (uint32_t)(systemtick + timer_period);
                timers[i].state = TIMER_RUNNING;

                result = i;
            }

            break;
        }
        else
        {
            /* do nothing */
        }

    }

    return result;
}

/**
 * @brief Reconfigures an existing timer.
 *
 * @param timer_id Timer ID to be configured.
 * @param timer_cbk New timer callback, if NULL the callback stills the old one.
 * @param timer_type New timer type, if KEEP_TIMER_TYPE the type stills the same.
 * @param timer_period New timer peridod, if zero the period stills the same.
 * This also changes timer timeout.
 */
void Timer_Config(uint16_t timer_id, timer_callback_t timer_cbk, \
    timer_type_t timer_type, uint32_t timer_period)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else if (TIMER_EMPTY != timers[timer_id].state)
    {
        if (NULL != timer_cbk)
        {
            timers[timer_id].cbk = timer_cbk;
        }
        else
        {
            /* do nothing */
        }


        if (KEEP_TIMER_TYPE > timer_type)
        {
            timers[timer_id].type = timer_type;
        }
        else
        {
            /* do nothing */
        }

        if (0 != timer_period)
        {
            timers[timer_id].period = timer_period;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }
}

/**
 * @brief Deletes an existing timer.
 *
 * @param timer_id Timer ID to be deleted.
 */
void Timer_Delete(uint16_t timer_id)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else
    {
        timers[timer_id].state = TIMER_EMPTY;
    }

}

/**
 * @brief Start a stopped timer.
 *
 * @param timer_id Timer ID.
 */
void Timer_Start(uint16_t timer_id)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else if (TIMER_EMPTY != timers[timer_id].state)
    {
        timers[timer_id].timeout = (uint32_t)(systemtick + timers[timer_id].period);
        timers[timer_id].state = TIMER_RUNNING;
    }
    else
    {
        /* do nothing */
    }
}

/**
 * @brief Stops a running timer.
 *
 * @param timer_id Timer ID.
 */
void Timer_Stop(uint16_t timer_id)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else if (TIMER_EMPTY != timers[timer_id].state)
    {
        timers[timer_id].state = TIMER_STOPPED;
    }
    else
    {
        /* do nothing */
    }
}

/**
 * @brief Get timer current state.
 *
 * @param timer_id Timer ID.
 * @return timer_state_t Timer state.
 */
timer_state_t Timer_GetTimerState(uint16_t timer_id)
{
    timer_state_t state = TIMER_ERROR;
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else
    {
        state = timers[timer_id].state;
    }

    return state;
}

/**
 * @brief Delay in millisseconds
 *
 * @param time_ms time to delay (ms)
 */
void Timer_Delay(uint32_t time_ms)
{
    delay_time = 0;
    while (delay_time < time_ms*TIME_1MS)
    {
        asm volatile("nop");
    }
}

/**
 * @brief Delay in multiples of 10us
 *
 * @param time_10us time to delay. Ex: time_10us = 5: delay for 50us.
 */
void Timer_Delay_10us(uint32_t time_10us)
{
    delay_time = 0;
    while (delay_time < time_10us*2)
    {
        asm volatile("nop");
    }
}


/**
 * @brief Delay in multiples of 5us
 *
 * @param time_5us time to delay. Ex: time_5us = 5: delay for 25us.
 */
void Timer_Delay_5us(uint32_t time_5us)
{

	delay_time = 0;

    while (delay_time < time_5us)
    {
        asm volatile("nop");
    }

}
