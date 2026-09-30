
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* The devicetree node identifiers for the aliases */
#define LED_NODE_RED DT_ALIAS(led0)
#define LED_NODE_BLUE DT_ALIAS(led1)

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_NODE_RED, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_NODE_BLUE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool is_red_turn = true;

    /* Check if devices are ready */
    if (!gpio_is_ready_dt(&led_red)) {
        LOG_ERR("Red LED device not ready");
        return 0;
    }
    if (!gpio_is_ready_dt(&led_blue)) {
        LOG_ERR("Blue LED device not ready");
        return 0;
    }

    /* Configure both as outputs, initially OFF (INACTIVE) */
    if (gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE) < 0) return 0;

    LOG_INF("Starting alternating blink loop...");

    while (1) {
        if (is_red_turn) {
            /* Set Red ACTIVE (1) and Blue INACTIVE (0) */
            gpio_pin_set_dt(&led_red, 1);
            gpio_pin_set_dt(&led_blue, 0);
            LOG_INF("LED state: RED is ON, BLUE is OFF");
        } else {
            /* Set Red INACTIVE (0) and Blue ACTIVE (1) */
            gpio_pin_set_dt(&led_red, 0);
            gpio_pin_set_dt(&led_blue, 1);
            LOG_INF("LED state: RED is OFF, BLUE is ON");
        }

        /* Swap turns for the next loop iteration */
        is_red_turn = !is_red_turn;
        
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}