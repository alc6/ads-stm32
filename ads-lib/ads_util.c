#include <stddef.h>
#include "ads_util.h"
#include "ads_assert.h"

/**@brief Function for decoding a int16 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @return      Decoded value.
 */
int16_t ads_int16_decode(const uint8_t * p_encoded_data)
{
	ASSERT(p_encoded_data != NULL);

        return ( (((uint16_t)(p_encoded_data)[0])) |
                 (((int16_t)(p_encoded_data)[1]) << 8 ));
}

/**@brief Function for decoding a uint16 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @return      Decoded value.
 */
uint16_t ads_uint16_decode(const uint8_t * p_encoded_data)
{
	ASSERT(p_encoded_data != NULL);

        return ( (((uint16_t)(p_encoded_data)[0])) |
                 (((uint16_t)(p_encoded_data)[1]) << 8 ));
}

/**@brief Function for encoding a uint16 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes written.
 */
uint8_t ads_uint16_encode(uint16_t value, uint8_t * p_encoded_data)
{
	ASSERT(p_encoded_data != NULL);

    p_encoded_data[0] = (uint8_t)  (value & 0x00FF);
    p_encoded_data[1] = (uint8_t) ((value & 0xFF00) >> 8);
    return sizeof(uint16_t);
}
