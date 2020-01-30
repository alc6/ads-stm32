#ifndef ADS_ERR_H_
#define ADS_ERR_H_

#define ADS_NEW_DATA		   (1)
#define ADS_OK                 (0)  /**< Success */
#define ADS_ERR                (-1) /**< General Error */
#define ADS_ERR_BAD_PARAM      (-2) /**< Bad parameter to an API call */
#define ADS_ERR_OP_IN_PROGRESS (-3) /**< Operation in progress */
#define ADS_ERR_IO             (-4) /**< Error communicating with ads */
#define ADS_ERR_DEV_ID         (-5) /**< Device ID does not match expected ID */
#define ADS_ERR_TIMEOUT        (-6) /**< Operation timed out */

#endif /* ADS_ERR_H_ */
