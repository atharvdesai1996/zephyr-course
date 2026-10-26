#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* Runs very early, before drivers (console UART not ready yet) */
void board_early_init_hook(void)
{
}

/* Runs after drivers are initialized, before main() */
void board_late_init_hook(void)
{
	printk("Board Initialized\n");
}