#define DT_DRV_COMPAT our_driver

#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

// Use instead of _DECLARE if you have another module in our_driver channel
// We can register only one, declare multiple times
LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

static int channel_get_my_impl(const struct device *dev,
                               enum sensor_channel channel,
                               struct sensor_value *value)
{
    LOG_INF("Hello from Channel get, channel %d Getting sensor value", channel);
    return 0;
}

static int channel_set_my_impl(const struct device *dev,
                               enum sensor_channel channel, 
                               const struct sensor_value *value)
{
    LOG_INF("Hello from Channel set, channel %d Setting sensor value", channel);
    return 0;
}


static DEVICE_API(sensor, api_iomico_lecture) = {
    .channel_get = channel_get_my_impl,
    //.channel_set = channel_set_my_impl,
};

// init function
static int init(const struct device *dev)
{
    LOG_INF("Hello from init function");
    return 0;
}

// Passing for power mgmt, stateless driver, pointer to init config for this driver
// Post kernel - init level ideal for sensors, I2C devices, UART
// Priority in the level - higher level means later
DEVICE_DT_INST_DEFINE(0, init, NULL, NULL, NULL, POST_KERNEL, 80, &api_iomico_lecture); 

