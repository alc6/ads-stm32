#ifndef ADS_H_
#define ADS_H_

#include <stdint.h>
#include <stdbool.h>
#include "ads_util.h"
#include "ads_err.h"

#define ADS_X_DATA 0
#define ADS_Y_DATA 1

typedef enum {
	ADS_CALIBRATE_FIRST = 0,
	ADS_CALIBRATE_FLAT,
	ADS_CALIBRATE_PERP,
	ADS_CALIBRATE_CLEAR
} ADS_CALIBRATION_STEP_T;


typedef enum {
	ADS_1_HZ   = 16384,
	ADS_10_HZ  = 1638,
	ADS_20_HZ  = 819,
	ADS_50_HZ  = 327,
	ADS_100_HZ = 163,
	ADS_200_HZ = 81,
	ADS_333_HZ = 49,
	ADS_500_HZ = 32,
} ADS_SPS_T;

typedef struct {
	ADS_SPS_T sps; /* Sampling rate */

	unsigned char i2c_address;

	bool interrupt_enabled;
	bool new_data_available; /* New data available: setup from interrupt */

	void*    reset_port;
	uint16_t reset_pin;


	void*    int_port;
	uint16_t int_pin;

	float ang[2];

} ads_init_t;


typedef struct {
	/* Settings */
	ADS_SPS_T sps; /* Sampling rate */
	ADS_DEV_IDS_T type;


	/* Communication */
	unsigned char i2c_address;
	bool interrupt_enabled;
	bool interrupt_mode;
	bool interrupt_newData;

	uint8_t comSize; // depends of the type

	/* Hardware */
	void*    reset_port;
	uint16_t reset_pin;

	void*    int_port;
	uint16_t int_pin;
	uint32_t int_irqn;

	float data[2];
	float filter_samples[2][6];
	float prev_sample[2];


} ads_handler_t;

/**
   @brief Initialize the ADS device
   @param ads_init Reference to a ads_handler_t structure
   @return  ADS_OK if successful ADS_ERR_IO if failed
*/
int ads_init(ads_handler_t * ads_init);

/**
   @brief Check the type of the device (1 or 2 axis), the type is written in the handler
   @param adsHandler Reference to a ads_handler_t structure
   @return  ADS_OK if successful ADS_ERR_IO if failed
*/
int ads_get_dev_id(ads_handler_t * adsHandler);

/**
   @brief Place a device in a continuous output mode
   @param adsHandler Reference to a ads_handler_t structure
   @param run Enable/Disable
   @return  ADS_OK if successful ADS_ERR_IO if failed
*/
int ads_run(ads_handler_t * adsHandler, bool run);

/**
   @brief Set the sample rate of a device
   @param adsHandler Reference to a ads_handler_t structure
   @param sps Reference to a ads_handler_t structure
   @return  ADS_OK if successful ADS_ERR_IO if failed
*/
int ads_set_sample_rate(ads_handler_t * adsHandler, ADS_SPS_T sps);

/**
   @brief Poll data from a device
   @param adsHandler Reference to a ads_handler_t structure
   @return  ADS_OK if successful ADS_ERR_IO if failed
*/
int ads_poll_data(ads_handler_t * adsHandler);

/**
   @brief Calibrates two axis ADS.
   @param ads_calibration_step  ADS_CALIBRATE_STEP_T to perform
   @param degrees uint8_t angle at which sensor is bent when performing
          ADS_CALIBRATE_FLAT, and ADS_CALIBRATE_PERP
   @return  ADS_OK if successful ADS_ERR_IO or ADS_BAD_PARAM if failed
*/
int ads_calibrate(ads_handler_t * adsHandler, ADS_CALIBRATION_STEP_T ads_calibration_step, uint8_t degrees);

/*
   @brief Places ADS in polled or sleep mode
   @param  run true if activating ADS, false is putting in suspend mode
   @return  ADS_OK if successful ADS_ERR_IO if failed
*/
int ads_hal_begin_polling_data(ads_handler_t * adsHandler, bool poll);

/**
   @brief Shutdown ADS. Requires reset to wake up from Shutdown
   @return  True if successful false if failed
*/
int ads_shutdown(ads_handler_t * adsHandler);

#endif /* ADS_H_ */
