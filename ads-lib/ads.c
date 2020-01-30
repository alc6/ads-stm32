#include <math.h>
#include <stddef.h>

#include "ads.h"
#include "ads_hal.h"
#include "ads_assert.h"
#include "string.h"


/**
 * @brief Initializes the hardware abstraction layer and sample rate of the ADS
 *
 * @param	ads_init_t	initialization structure of the ADS
 * @return	ADS_OK if successful ADS_ERR if failed
 */
int ads_init(ads_handler_t * adsHandler)
{
	ASSERT(adsHandler != NULL);

	 //default
	//Check the comSize;

	// Configure I2C bus
	//TODO Next, for the moment handled by CubeMX

	// Configure INT pin
	//TODO Next, for the moment handled by CubeMX

	ads_shutdown(adsHandler);

	// Temp (TODO)
	// Make sure interrupt is disabled first
	if (adsHandler->interrupt_mode)
	{
		ads_hal_int_disable(adsHandler->int_irqn, &(adsHandler->interrupt_enabled));
	}

	// Reset the ads
	ads_hal_reset_device((GPIO_TypeDef*) adsHandler->reset_port, adsHandler->reset_pin);

	// Wait for ads to initialize
	ads_hal_delay(500);

	// Enable INT pin interrupt
	//TODO Next, for the moment only polling

	// Make sure the device is not streaming data
	ads_run(adsHandler, 0);

	// Check that the device id matched ADS_TWO_AXIS
	if(ads_get_dev_id(adsHandler) != ADS_OK)
	{
		//return ADS_ERR_DEV_ID;
	}
	else
	{
		/* Check the type */

	}

#if ADS_DFU_CHECK
 	if(ads_two_axis_dfu_check((uint8_t)ADS_GET_FW_VER))
	{
		ads_two_axis_dfu_reset();
		ads_hal_delay(100);		// Give ADS time to reset
 		if(ads_two_axis_dfu_update() != ADS_OK)
			return ADS_ERR;
		ads_hal_delay(200);	// Let it reinitialize
	}
#endif

	if(ads_set_sample_rate(adsHandler, adsHandler->sps))
	{
		//return ADS_ERR;
	}
	else
	{
		//return ADS_OK;
	}

	//ads_hal_delay(20);

	ads_hal_begin_polling_data(adsHandler, 1);

	return ADS_OK;
}



int ads_get_dev_id(ads_handler_t * adsHandler)
{
	ASSERT(adsHandler != NULL);

	uint8_t buffer[ADS_TRANSFER_SIZE_TWO_AXIS];

	memset(buffer, 0, ADS_TRANSFER_SIZE_TWO_AXIS);

	buffer[0] = ADS_GET_DEV_ID;

	// Disable interrupt to prevent callback from reading out device id
	if (adsHandler->interrupt_mode)
	{
		ads_hal_int_disable(adsHandler->int_irqn, &(adsHandler->interrupt_enabled));
	}

	ads_hal_write_buffer(adsHandler->i2c_address, buffer, adsHandler->comSize);
	ads_hal_delay(2);
	ads_hal_read_buffer(adsHandler->i2c_address, buffer, adsHandler->comSize);


	if (adsHandler->interrupt_mode)
	{
		ads_hal_int_enable(adsHandler->int_irqn, &(adsHandler->interrupt_enabled));
	}

	/* Check that packet read is a device id packet and that
	 * and that the device id is a two axis sensor */
 	if(buffer[0] == ADS_DEV_ID)
	{
		if ((buffer[1] == ADS_ONE_AXIS) || (buffer[1] == ADS_TWO_AXIS))
		{
			if (buffer[1] == ADS_ONE_AXIS)
			{
				adsHandler->comSize = ADS_TRANSFER_SIZE_ONE_AXIS;
			}
			adsHandler->type = buffer[1];
			return ADS_OK;
		}
	}
	// Else it went wrong : either first byte is not dev_id, either second byte is not ADS_ONE_AXIS or ADS_TWO_AXIS
	return ADS_ERR_DEV_ID;
}

int ads_run(ads_handler_t * adsHandler, bool run)
{
	ASSERT(adsHandler != NULL);

	uint8_t buffer[ADS_TRANSFER_SIZE_TWO_AXIS];
	memset(buffer, 0, ADS_TRANSFER_SIZE_TWO_AXIS);

	buffer[0] = ADS_RUN;
	buffer[1] = run;

	return ads_hal_write_buffer(adsHandler->i2c_address ,buffer, adsHandler->comSize);
}

int ads_set_sample_rate(ads_handler_t * adsHandler, ADS_SPS_T sps)
{
	ASSERT(adsHandler != NULL);

	uint8_t buffer[ADS_TRANSFER_SIZE_TWO_AXIS];
	memset(buffer, 0, ADS_TRANSFER_SIZE_TWO_AXIS);

	buffer[0] = ADS_SPS;
	ads_uint16_encode(sps, &buffer[1]);

	return ads_hal_write_buffer(adsHandler->i2c_address ,buffer, adsHandler->comSize);
}


int ads_poll_data(ads_handler_t * adsHandler)
{
	ASSERT(adsHandler != NULL);

	uint8_t buffer[ADS_TRANSFER_SIZE_TWO_AXIS];
	memset(buffer, 0, ADS_TRANSFER_SIZE_TWO_AXIS);

	if (adsHandler->interrupt_mode)
	{
		/* Working with interrupts */
		if (adsHandler->interrupt_newData)
		{
			// TODO
			return ADS_ERR_BAD_PARAM;
		}
	}
	else
	{
		/* Poll */
		if(ads_hal_read_buffer(adsHandler->i2c_address , buffer, adsHandler->comSize) == ADS_OK)
		{
			if(buffer[0] == ADS_SAMPLE)
			{
				if (adsHandler->type == ADS_ONE_AXIS)
				{
					int16_t temp = ads_int16_decode(&buffer[1]);
					adsHandler->data[ADS_X_DATA] = (float)temp/64.0f;
				}
				else if (adsHandler->type == ADS_TWO_AXIS)
				{
					int16_t temp = ads_int16_decode(&buffer[1]);
					adsHandler->data[ADS_X_DATA] = (float)temp/32.0f;

					temp = ads_int16_decode(&buffer[3]);
					adsHandler->data[ADS_Y_DATA] = (float)temp/32.0f;
				}

				return ADS_NEW_DATA;
			}
		}
		else
		{
			return ADS_ERR_IO;
		}

	}
	return ADS_ERR;
}


int ads_calibrate(ads_handler_t * adsHandler, ADS_CALIBRATION_STEP_T ads_calibration_step, uint8_t degrees)
{
	ASSERT(adsHandler != NULL);

	uint8_t buffer[ADS_TRANSFER_SIZE_TWO_AXIS];
	memset(buffer, 0, ADS_TRANSFER_SIZE_TWO_AXIS);

	buffer[0] = ADS_CALIBRATE;
	buffer[1] = ads_calibration_step;
	buffer[2] = degrees;

	return ads_hal_write_buffer(adsHandler->i2c_address ,buffer, adsHandler->comSize);
}


int ads_hal_begin_polling_data(ads_handler_t * adsHandler, bool poll)
{
	ASSERT(adsHandler != NULL);

    uint8_t buffer[ADS_TRANSFER_SIZE_TWO_AXIS];
    memset(buffer, 0, ADS_TRANSFER_SIZE_TWO_AXIS);

    buffer[0] = ADS_INTERRUPT_ENABLE;
    buffer[1] = poll;

  return ads_hal_write_buffer(adsHandler->i2c_address, buffer, adsHandler->comSize);
}

int ads_shutdown(ads_handler_t * adsHandler)
{
	ASSERT(adsHandler != NULL);

    uint8_t buffer[ADS_TRANSFER_SIZE_TWO_AXIS];
    memset(buffer, 0, ADS_TRANSFER_SIZE_TWO_AXIS);

    buffer[0] = ADS_SHUTDOWN;

    return ads_hal_write_buffer(adsHandler->i2c_address ,buffer, adsHandler->comSize);
}


void ads_signalFilter(ads_handler_t * adsHandler)
{
	ASSERT(adsHandler != NULL);

    uint8_t axisAmount = adsHandler->type;

    for (uint8_t i = 0; i < axisAmount; i++)
    {
      adsHandler->filter_samples[i][5] = adsHandler->filter_samples[i][4];
      adsHandler->filter_samples[i][4] = adsHandler->filter_samples[i][3];
      adsHandler->filter_samples[i][3] = (float)adsHandler->data[i];
      adsHandler->filter_samples[i][2] = adsHandler->filter_samples[i][1];
      adsHandler->filter_samples[i][1] = adsHandler->filter_samples[i][0];

      // 20 Hz cutoff frequency @ 100 Hz Sample Rate
      adsHandler->filter_samples[i][0] = adsHandler->filter_samples[i][1] * (0.36952737735124147f) - 0.19581571265583314f * adsHandler->filter_samples[i][2] +
                           0.20657208382614792f * (adsHandler->filter_samples[i][3] + 2 * adsHandler->filter_samples[i][4] + adsHandler->filter_samples[i][5]);

      adsHandler->data[i] = adsHandler->filter_samples[i][0];
  }
}

// Deadzone filter
void ads_deadzoneFilter(ads_handler_t * adsHandler)
{
	ASSERT(adsHandler != NULL);

    uint8_t axisAmount = adsHandler->type;
    float dead_zone = 0.5f;

    for (uint8_t i = 0; i < axisAmount; i++)
    {
      if (fabs(adsHandler->data[i] - adsHandler->prev_sample[i]) > dead_zone)
      	  adsHandler->prev_sample[i] = adsHandler->data[i];
      else
      	  adsHandler->data[i] = adsHandler->prev_sample[i];
    }
}




