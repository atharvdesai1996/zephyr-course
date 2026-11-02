#define DT_DRV_COMPAT our_driver

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

// Use instead of _DECLARE if you have another module in our_driver channel
// We can register only one, declare multiple times
LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

/* Per-instance constant config (from devicetree, lives in flash) */
struct our_driver_config {
    struct gpio_dt_spec led;
};

/* Per-instance mutable runtime data (lives in RAM) */
struct our_driver_data {
    int led_state; /* last state written to the LED (1 = on, 0 = off) */
};

/*
 * sensor_sample_fetch() -> sample_fetch
 * In a real sensor this talks to the hardware (I2C/SPI/ADC...), triggers a
 * measurement and caches the raw result in dev->data. No value is returned
 * to the caller here. For this task, "fetching" = turning the LED ON.
 */
static int sample_fetch_my_impl(const struct device *dev, enum sensor_channel channel)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *data = dev->data;

    if (channel != SENSOR_CHAN_ALL && channel != SENSOR_CHAN_AMBIENT_TEMP) {
        return -ENOTSUP;
    }

    int ret = gpio_pin_set_dt(&cfg->led, 1);
    if (ret < 0) {
        LOG_ERR("%s: failed to set LED (%d)", dev->name, ret);
        return ret;
    }
    data->led_state = 1;

    LOG_INF("%s: sample_fetch -> LED ON", dev->name);
    return 0;
}

/*
 * sensor_channel_get() -> channel_get
 * In a real sensor this only converts the cached data from dev->data into a
 * struct sensor_value, without touching the hardware. For this task it turns
 * the LED OFF and reports the state it had (as fetched) in value.
 */
static int channel_get_my_impl(const struct device *dev,
                               enum sensor_channel channel,
                               struct sensor_value *value)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *data = dev->data;

    if (channel != SENSOR_CHAN_AMBIENT_TEMP) {
        return -ENOTSUP;
    }

    value->val1 = data->led_state; /* report the state the LED had */
    value->val2 = 0;

    int ret = gpio_pin_set_dt(&cfg->led, 0);
    if (ret < 0) {
        LOG_ERR("%s: failed to clear LED (%d)", dev->name, ret);
        return ret;
    }
    data->led_state = 0;

    LOG_INF("%s: channel_get (channel %d) -> LED OFF", dev->name, channel);
    return 0;
}

static DEVICE_API(sensor, api_iomico_lecture) = {
    .sample_fetch = sample_fetch_my_impl,
    .channel_get = channel_get_my_impl,
};

// init function
static int init(const struct device *dev)
{
    const struct our_driver_config *cfg = dev->config;

    if (!gpio_is_ready_dt(&cfg->led)) {
        LOG_ERR("%s: LED GPIO not ready", dev->name);
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    LOG_INF("%s: init done", dev->name);
    return 0;
}

// Post kernel - init level ideal for sensors, I2C devices, UART
// Priority in the level - higher number means later
#define DEV_INST(inst)                                                         \
    static const struct our_driver_config config_##inst = {                    \
        .led = GPIO_DT_SPEC_INST_GET(inst, led_gpios),                         \
    };                                                                         \
    static struct our_driver_data data_##inst;                                 \
    DEVICE_DT_INST_DEFINE(inst, init, NULL, &data_##inst, &config_##inst,      \
                          POST_KERNEL, 80, &api_iomico_lecture);

// Find all okay instances in the devicetree and instantiate a driver for each
DT_INST_FOREACH_STATUS_OKAY(DEV_INST)
