#ifndef STM32F030_HW_GPIO_H_
#define STM32F030_HW_GPIO_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f0xx.h"

typedef enum
{
	MODE_INPUT = 0,
	MODE_OUTPUT_PUSH_PULL,
	MODE_OUTPUT_OPEN_DRAIN,
	MODE_AF,
	MODE_ANALOG
}gpio_mode_e;

typedef enum
{
	OSPEED_LOW = 0,
	OSPEED_MED,
	OSPEED_HI
}gpio_ospeed_e;

typedef enum
{
	PUPD_NOPULL = 0,
	PUPD_PULLUP,
	PUPD_PULLDOWN,
}gpio_pupd_e;

typedef enum
{
	AF0 = 0,
	AF1,
	AF2,
	AF3,
	AF4,
	AF5,
	AF6,
	AF7,
}gpio_af_e;

typedef enum
{
	GPIO_PIN_STATE_LOW = 0,
	GPIO_PIN_STATE_HIGH,

	GPIO_PIN_STATE_UNKNOWN
}gpio_pin_state_e;

void Gpio_Config_MODE(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_mode_e MODE_x);
void Gpio_Config_OSPEED(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_ospeed_e OSPEED_x);
void Gpio_Config_PUPD(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_pupd_e PUPD_x);
void Gpio_Config_AF(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_af_e AFx);

void Gpio_Digital_Write(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_pin_state_e GPIO_PIN_STATE_x);
void Gpio_Digital_Toggle(GPIO_TypeDef *GPIOx, uint8_t Pin);
gpio_pin_state_e Gpio_Digital_Read(GPIO_TypeDef *GPIOx, uint8_t Pin);

#ifdef  __cplusplus
}
#endif

#endif /* STM32F030_HW_GPIO_H_ */
