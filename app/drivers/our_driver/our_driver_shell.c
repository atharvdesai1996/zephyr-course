#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/iterable_sections.h>

#include "our_driver.h"

/*
 * Find a device by name WITHOUT checking whether it is ready.
 * device_get_binding() returns NULL for devices whose init() failed, so it
 * could never report "ready: no". Walking the device list can.
 */
static const struct device *find_device(const char *name)
{
    STRUCT_SECTION_FOREACH(device, dev) {
        if (dev->name != NULL && strcmp(dev->name, name) == 0) {
            return dev;
        }
    }
    return NULL;
}

/* Same lookup, but also requires the device to be ready (for fetch/read) */
static const struct device *get_ready_device(const struct shell *sh, const char *name)
{
    const struct device *dev = find_device(name);

    if (dev == NULL) {
        shell_error(sh, "Device '%s' not found", name);
        return NULL;
    }
    if (!device_is_ready(dev)) {
        shell_error(sh, "Device '%s' is not ready", name);
        return NULL;
    }
    return dev;
}

/* sensor fetch <dev>  ->  sensor_sample_fetch()  (our driver: LED ON) */
static int cmd_fetch_handler(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_ready_device(sh, argv[1]);

    if (dev == NULL) {
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch failed: %d", ret);
        return ret;
    }
    shell_print(sh, "%s: sample fetched", dev->name);
    return 0;
}

/* sensor read <dev>  ->  sensor_channel_get()  (our driver: LED OFF) */
static int cmd_read_handler(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = get_ready_device(sh, argv[1]);

    if (dev == NULL) {
        return -ENODEV;
    }

    /* Our driver only supports SENSOR_CHAN_AMBIENT_TEMP */
    struct sensor_value value;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &value);
    if (ret < 0) {
        shell_error(sh, "sensor_channel_get failed: %d", ret);
        return ret;
    }
    shell_print(sh, "%s: val1=%d val2=%d", dev->name, value.val1, value.val2);
    return 0;
}

/* sensor info <dev>  ->  device name + ready state */
static int cmd_info_handler(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = find_device(argv[1]);

    if (dev == NULL) {
        shell_error(sh, "Device '%s' not found", argv[1]);
        return -ENODEV;
    }

    shell_print(sh, "name : %s", dev->name);
    shell_print(sh, "ready: %s", device_is_ready(dev) ? "yes" : "no");

    /* Extra info only for our_driver devices (custom API) */
    struct our_driver_info info;
    if (our_driver_get_info(dev, &info) == 0) {
        shell_print(sh, "led  : %s (%s pin %d)", info.led_state ? "on" : "off",
                    info.led_port, info.led_pin);
        shell_print(sh, "level: %d", info.level);
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcmds,
    SHELL_CMD_ARG(fetch, NULL, "Fetch a sample.  Usage: sensor fetch <device>",
                  cmd_fetch_handler, 2, 0),
    SHELL_CMD_ARG(read, NULL, "Read channel value.  Usage: sensor read <device>",
                  cmd_read_handler, 2, 0),
    SHELL_CMD_ARG(info, NULL, "Device name and ready state.  Usage: sensor info <device>",
                  cmd_info_handler, 2, 0),
    SHELL_SUBCMD_SET_END
);

/* Root command "sensor" (argc counts the command name: 2 = name + <device>) */
SHELL_CMD_REGISTER(sensor, &sensor_subcmds, "Sensor commands (L07 Task 1)", NULL);