#include "ads_hal.h"
#include "ads_assert.h"

/* Hardware Specific Includes */
/* Here the 'i2c.h' file : if it's missing, make sure an I2C peripheral is enabled, and check,
 * In STM32Cube perspective:
 * Project Manager > Code Generator > Generated files panel > Enable the pair files
 */
#include "i2c.h"
#include "gpio.h"
#include "cmsis_os2.h"

#define ADS_RST_DELAY 100 /* In milliseconds */

static I2C_HandleTypeDef * local_pHandler; /* Local handler */

/* TODO: Use definition to allow the user to specify if he is using RTOS and adapt functions */
void ads_hal_delay(uint32_t delay_ms)
{
	//HAL_Delay(delay_ms);
	//osDelay(pdMS_TO_TICKS(delay_ms));
	osDelay(delay_ms);
}

void ads_hal_int_enable(IRQn_Type IRQn, bool * status)
{
	if (!(*status))
	{
		HAL_NVIC_EnableIRQ(IRQn);
		*status = 1;
	}
}

void ads_hal_int_disable(IRQn_Type IRQn, bool * status)
{
	if (!status)
	{
		/* Error on pointer */
	}
	if (*status)
	{
		HAL_NVIC_DisableIRQ(IRQn);
		*status = 0;
	}
}

int ads_hal_write_buffer(uint8_t address, uint8_t * buffer, uint8_t len)
{
	HAL_StatusTypeDef res;

	res = HAL_I2C_Master_Transmit(&hi2c1, address << 1 , buffer, len, 0xffff);
#if 0
	res = HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(address<<1), buffer[0], 1, buffer+1, len-1, 0xff);
#endif
	if(res == HAL_OK)
	{
		return ADS_OK;
	}
	else
	{
		return ADS_ERR_IO;
	}
}

int ads_hal_read_buffer(uint8_t address, uint8_t * buffer, uint8_t len)
{
	HAL_StatusTypeDef res;

	res = HAL_I2C_Master_Receive(&hi2c1, address << 1, buffer, len, 0xffff);

	if(res == HAL_OK)
	{
		return ADS_OK;
	}
	else
	{
		return ADS_ERR_IO;
	}
}

void ads_hal_set_i2c_handler(I2C_HandleTypeDef * pHandler)
{
	if (!pHandler)
	{
		// Issue on the pointer
	}
	else
	{
		local_pHandler = pHandler;
	}
}

void ads_hal_reset_device(GPIO_TypeDef * port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	ads_hal_delay(ADS_RST_DELAY);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}


