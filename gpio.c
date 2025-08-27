#include "gpio.h"

//============   PRIVATE FUNCTIONS   ================//

static void _gpio_config_clock(GPIO_TypeDef *GPIOx)
{
	switch((uint32_t)(GPIOx))
	{
		case (uint32_t)GPIOA: RCC->AHBENR |= RCC_AHBENR_GPIOAEN; break;

		case (uint32_t)GPIOB: RCC->AHBENR |= RCC_AHBENR_GPIOBEN; break;

		case (uint32_t)GPIOC: RCC->AHBENR |= RCC_AHBENR_GPIOCEN; break;

		case (uint32_t)GPIOD: RCC->AHBENR |= RCC_AHBENR_GPIODEN; break;

		case (uint32_t)GPIOF: RCC->AHBENR |= RCC_AHBENR_GPIOFEN; break;

		default: break;
	}
}// end _gpio_config_clock


//=============   PUBLIC FUNCTIONS   ================//

void Gpio_Config_MODE(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_mode_e MODE_x)
{
	// Sanity Check:
	if(GPIOx == 0 || Pin > 15)
		return;

	uint8_t mask = 0;

	// Enable corresponding clock
	_gpio_config_clock(GPIOx);

	switch(MODE_x)
	{
		case MODE_OUTPUT_OPEN_DRAIN:
			mask = (0b01);
			// Config OTYPE as Open Drain
			GPIOx->OTYPER |= (1 << Pin);
			break;

		case MODE_OUTPUT_PUSH_PULL:
			mask = (0b01);
			// Reset OTYPE to Push Pull
			GPIOx->OTYPER &= ~(1 << Pin);
			break;

		case MODE_INPUT:	mask = (0b00); break;

		case MODE_AF:		mask = (0b10); break;

		case MODE_ANALOG:	mask = (0b11); break;

		default:	return;
	}

	// Reset GPIO Register Bits
	GPIOx->MODER &= ~(0b11 << (Pin*2));
	// Configure GPIO Register Bits
	GPIOx->MODER |= (mask << (Pin*2));

}// end Gpio_Config_Mode

void Gpio_Config_OSPEED(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_ospeed_e OSPEED_x)
{
	// Sanity Check:
	if(GPIOx == 0 || Pin > 15)
		return;

	uint8_t mask = 0;

	switch(OSPEED_x)
	{
		case OSPEED_LOW:	mask = 0b00; break;
		case OSPEED_MED: 	mask = 0b01; break;
		case OSPEED_HI:		mask = 0b11; break;
		default: return;
	}

	// Reset GPIO Register Bits
	GPIOx->OSPEEDR &= ~(0b11 << (Pin*2));
	// Configure GPIO Register Bits
	GPIOx->OSPEEDR |= (mask << (Pin*2));

}// end Gpio_Config_OSPEED


void Gpio_Config_PUPD(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_pupd_e PUPD_x)
{
	// Sanity Check:
	if(GPIOx == 0 || Pin > 15)
		return;

	uint8_t mask = 0;

	switch(PUPD_x)
	{
		case PUPD_NOPULL: 	mask = 0b00; break;
		case PUPD_PULLUP: 	mask = 0b01; break;
		case PUPD_PULLDOWN:	mask = 0b10; break;
		default: return;
	}

	// Reset GPIO Register Bits:
	GPIOx->PUPDR &= ~(0b11 << (Pin*2));
	// Config PUPD Register
	GPIOx->PUPDR |= (mask << (Pin*2));

}

void Gpio_Config_AF(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_af_e AFx)
{
	// Sanity Check:
	if(GPIOx == 0 || Pin > 15)
		return;

	uint8_t mask = 0;

	switch(AFx)
	{
		case AF0: mask = 0x00; break;
		case AF1: mask = 0x01; break;
		case AF2: mask = 0x02; break;
		case AF3: mask = 0x03; break;
		case AF4: mask = 0x04; break;
		case AF5: mask = 0x05; break;
		case AF6: mask = 0x06; break;
		case AF7: mask = 0x07; break;
		default: return;
	}

	if(Pin > 7)	// Register HIGH
	{
		// Reset AFR bits
		GPIOx->AFR[1] &= ~( 0b1111 << ((Pin-8)*4) );
		// Configure AFT bits
		GPIOx->AFR[1] |=  ( mask << ((Pin-8)*4) );
	}
	else		// Register LOW
	{
		// Reset AFR bits
		GPIOx->AFR[0] &= ~( 0b1111 << (Pin*4) );
		// Configure AFT bits
		GPIOx->AFR[0] |=  (mask << (Pin*4));
	}

}// end Gpio_Config_AF

void Gpio_Digital_Write(GPIO_TypeDef *GPIOx, uint8_t Pin, gpio_pin_state_e GPIO_PIN_STATE_x)
{
	// Sanity Check:
	if(GPIOx == 0 || Pin > 15)
		return;

	if(GPIO_PIN_STATE_x == GPIO_PIN_STATE_HIGH)
	{
		GPIOx->BSRR |= (1<<Pin);
	}
	else if(GPIO_PIN_STATE_x == GPIO_PIN_STATE_LOW)
	{
		GPIOx->BRR |= (1<<Pin);
	}
}

void Gpio_Digital_Toggle(GPIO_TypeDef *GPIOx, uint8_t Pin)
{
	// Sanity Check:
	if(GPIOx == 0 || Pin > 15)
		return;

	GPIOx->ODR ^= (1<<Pin);
}

gpio_pin_state_e Gpio_Digital_Read(GPIO_TypeDef *GPIOx, uint8_t Pin)
{
	// Sanity Check:
	if(GPIOx == 0 || Pin > 15)
		return GPIO_PIN_STATE_UNKNOWN;

	return ((GPIOx->IDR & (1<<Pin)) >> Pin) ? GPIO_PIN_STATE_HIGH : GPIO_PIN_STATE_LOW;
}
