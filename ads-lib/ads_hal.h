#ifndef ADS_HAL_H_
#define ADS_HAL_H_

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "ads_err.h"

#define ADS_TRANSFER_SIZE_ONE_AXIS		(3) // Transfer size for one axis sensors
#define ADS_TRANSFER_SIZE_TWO_AXIS		(5) // Transfer size for two axis sensors

#define ADS_COUNT				(10)		// Number of ADS devices attached to bus


/**
 * @brief Method used to execute a delay in the library
 *	Modify it based on your platform.
 * @param delay_ms	Delay to execute in milliseconds
 */
void ads_hal_delay(uint32_t delay_ms);

/**
 * @brief Method used to enable an interruption on a specific IRQ channel
 *	Modify it based on your platform.
 * @param IRQn	Interruption Channel
 * @param status Reference to a status boolean to save the actual state of interrupt channel
 */
void ads_hal_int_enable(IRQn_Type IRQn, bool * status);

/**
 * @brief Method used to disable an interruption on a specific IRQ channel
 *	Modify it based on your platform.
 * @param IRQn	Interruption Channel
 * @param status Reference to a status boolean to save the actual state of interrupt channel
 */
void ads_hal_int_disable(IRQn_Type IRQn, bool * status);

/**
 * @brief Write a buffer to a the device (I2C)
 *
 * @param address	I2C Address of the device (not shifted!)
 * @param buffer	Reference to the buffer to write
 * @return	ADS_OK if successful ADS_ERR_IO if failed
 */
int ads_hal_write_buffer(uint8_t address, uint8_t * buffer, uint8_t len);

/**
 * @brief Read buffer of data from the Angular Displacement Sensor
 *
 * @param buffer[out]	Read buffer
 * @param len			Length of buffer.
 * @return	ADS_OK if successful ADS_ERR_IO if failed
 */
int ads_hal_read_buffer(uint8_t address, uint8_t * buffer, uint8_t len);

/**
 * @brief Perform a hardware reset of the device
 * @param port GPIO's port of the reset signal
 * @param pin GPIO's pin of the reset signal
 */
void ads_hal_reset_device(GPIO_TypeDef * port, uint16_t pin);

/**
 * @brief Set the local I2C handler
 * @param pHandler Reference of the I2C Handler
 */
void ads_hal_set_i2c_handler(I2C_HandleTypeDef * pHandler);



/* THE FOLLOWING FUNCTIONS ARE NOT IMPLEMENTED YET ! */
//TODO
/**
 * @brief Selects the current device address the driver is communicating with
 *
 * @param device select device 0 - ADS_COUNT
 * @return	ADS_OK if successful ADS_ERR_BAD_PARAM if invalid device number
 */
int ads_hal_select_device(uint8_t device);

/**
 * @brief Updates the I2C address in the ads_addrs[] array. Updates the current
 *		  selected address.
 *
 * @param	device	device number of the device that is being updated
 * @param	address	new address of the ADS
 * @return	ADS_OK if successful ADS_ERR_BAD_PARAM if failed
 */
int ads_hal_update_device_addr(uint8_t device, uint8_t address);

/**
 * @brief Gets the current i2c address that the hal layer is addressing.
 *				Used by device firmware update (dfu)
 * @return	uint8_t _address
 */
uint8_t ads_hal_get_address(void);

/**
 * @brief Sets the current i2c address that the hal layer is addressing.
 *				Used by device firmware update (dfu)
 */
void ads_hal_set_address(uint8_t address);

#endif /* ADS_HAL_H_ */
