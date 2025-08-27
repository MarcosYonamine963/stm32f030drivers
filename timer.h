#ifndef STM32F030_HW_TIMER_H_
#define STM32F030_HW_TIMER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SECONDS_IN_A_HOUR    3600UL
#define SECONDS_IN_A_DAY     24 * SECONDS_IN_A_HOUR
#define SECONDS_IN_A_MONTH   30 * SECONDS_IN_A_DAY
#define SECONDS_IN_A_YEAR    365 * SECONDS_IN_A_DAY

#define TIMEBASE 	(200000UL) 					// usdo para configurar o systick. 200000: interrupt a cada 5 us.

// Defines for timer_period
#define TIME_1MS 	(TIMEBASE/1000UL)
#define TIME_1S		(TIME_1MS*1000UL)
#define TIME_1MIN	(TIME_1S*60UL)
#define TIME_1H		(TIME_1MIN*60UL)

/**
 * @brief Types of timer.
 *
 */
typedef enum
{
    ONE_SHOT_TIMER = 0,
    AUTO_RELOAD_TIMER,
    KEEP_TIMER_TYPE,
    TYPE_NOT_DEFINED,
}
timer_type_t;

/**
 * @brief Types of timer state.
 *
 */
typedef enum
{
    TIMER_STOPPED = 0,
    TIMER_RUNNING,
    TIMER_EMPTY,
    TIMER_ERROR = 0xFF,
}
timer_state_t;

/**
 * @brief Timer callback definition.
 *
 * The callback functions must accept a void pointer and a uint32_t varible as
 * parameters.
 *
 * The timer library will pass the current systick value on ptr parameter.
 *
 */
typedef void (*timer_callback_t)(void* ptr, uint32_t size);


/**
 * @brief Initialize timer library.
 *
 * @param tick Tick value for interruption, if zero it will be used 1000 as value.
 */
void Timer_Init(void);

/**
 * @brief Timer State Machine
 *
 */
void Timer_SM(void);

/**
 * @brief Get current System tick value.
 *
 * @return uint32_t System tick.
 */
uint32_t Timer_GetSystemTick(void);

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
    uint32_t timer_period);

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
    timer_type_t timer_type, uint32_t timer_period);


/**
 * @brief Deletes an existing timer.
 *
 * @param timer_id Timer ID to be deleted.
 */
void Timer_Delete(uint16_t timer_id);

/**
 * @brief Start a stopped timer.
 *
 * @param timer_id Timer ID.
 */
void Timer_Start(uint16_t timer_id);

/**
 * @brief Stops a running timer.
 *
 * @param timer_id Timer ID.
 */
void Timer_Stop(uint16_t timer_id);

/**
 * @brief Get timer current state.
 *
 * @param timer_id Timer ID.
 * @return timer_state_t Timer state.
 */
timer_state_t Timer_GetTimerState(uint16_t timer_id);

/**
 * @brief Delay in millisseconds
 *
 * @param time_ms time to delay (ms)
 */
void Timer_Delay(uint32_t time_ms);

/**
 * @brief Delay in multiples of 10us
 *
 * @param time_10us time to delay. Ex: time_10us = 5: delay for 50us.
 */
void Timer_Delay_10us(uint32_t time_10us);

/**
 * @brief Delay in multiples of 5us
 *
 * @param time_5us time to delay. Ex: time_5us = 5: delay for 25us.
 */
void Timer_Delay_5us(uint32_t time_5us);


#ifdef  __cplusplus
}
#endif

#endif /* STM32F030_HW_TIMER_H_ */
