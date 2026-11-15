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

/** Driver-specific info reported by our_driver_get_info() */
struct our_driver_info {
    int led_state;      /* 1 = LED on (fetched), 0 = off */
    int level;          /* value set via our_driver_set_level() */
    const char *led_port; /* name of the GPIO controller driving the LED */
    int led_pin;        /* pin number on that port */
};

/**
 * @brief Custom extension API: read driver-specific info.
 * @return 0 on success, -EINVAL if dev is not an our_driver instance
 */
int our_driver_get_info(const struct device *dev, struct our_driver_info *info);

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVER_H_ */
