#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Custom extension API: set the "level" stored in the driver's dynamic data.
 *
 * Works on any our_driver instance; the level is reported in sensor_value.val2
 * by sensor_channel_get().
 *
 * @param dev   our_driver instance
 * @param level new level value
 * @return 0 on success, negative errno otherwise
 */
int our_driver_set_level(const struct device *dev, int level);

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVER_H_ */
