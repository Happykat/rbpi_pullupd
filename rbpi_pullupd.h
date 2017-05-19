#ifndef RBPI_PULLUPD_H
#define RBPI_PULLUPD_H

#include <linux/types.h>

/*
 * Define the class to register to
 */
#define CLASS_NAME "gpio_pud"

/*
 * Each register holds 32 bit
 */
#define BITS_PER_REGISTER (1 << 5)

/*
 * define the adress at wich the GPPUD register starts
 * the bcm2835 / bcm2836 has 41 GPIO registers , the registers managin the gpio
 * pull up or pull down are starting at the 37th register. Each register is 32
 * bit wide.
 */
#define GPPUD_OFFSET 37 * BITS_PER_REGISTER / 8


/*
 * Defines commands to enable pull up , pull down or reset on a specific gpio
 */
#define PULL_NONE	(0 << 0)
#define PULL_DOWN	(1 << 0)
#define PULL_UP		(1 << 1)
/*
 * Define the number of cycle to wait for the gpio chip to acknoledge the pull
 * up , pull down or reset command.
 */
#define CYCLE_TO_WAIT	150

/*
 * Define the value to enable or disable the clock for a specific gpio
 */
#define ENABLE_CLOCK	(1 << 0)
#define DISABLE_CLOCK	(0 << 0)

/*
 * Define the structure representing the GPIO pull up / down registers for the
 * rbpi bcm2836/bcm2836
 */
struct rbpi_pullupd
{
	volatile uint32_t gppud;
	volatile uint32_t gppudclk[2];
	uint32_t unused;
};

int gpio_pull_up(int gpio);
int gpio_pull_down(int gpio);
int gpio_reset(int gpio);

void __set_gpud(uint32_t value);
void __setgpudclk(int gpio, uint32_t value);

#endif
