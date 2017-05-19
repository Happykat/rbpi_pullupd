#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <mach/platform.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "rbpi_pullupd.h"

MODULE_LICENSE("GPL");

static ssize_t gpio_sysfs_pull_up(struct class *class,
				  struct class_attribute *attr, const char *buf,
				  size_t count);

static ssize_t gpio_sysfs_pull_down(struct class *class,
				    struct class_attribute *attr,
				    const char *buf, size_t count);

static ssize_t gpio_sysfs_reset(struct class *kobj,
				struct class_attribute *attr,
				const char *buf, size_t count);

static ssize_t gpio_rbpi_pullupd(struct class *, struct class_attribute *, char *);

static struct class_attribute gpio_pud_class_attrs[] = {
	__ATTR(pull_up, 0770, gpio_rbpi_pullupd, gpio_sysfs_pull_up),
	__ATTR(pull_down, 0770, gpio_rbpi_pullupd, gpio_sysfs_pull_down),
	__ATTR(reset, 0770, gpio_rbpi_pullupd, gpio_sysfs_reset),
	__ATTR_NULL,
};

static struct class gpio_pud_class =  {
	.name = CLASS_NAME,
	.class_attrs = gpio_pud_class_attrs,
};

volatile struct rbpi_pullupd *registers = (struct rbpi_pullupd*)
	(__io_address(GPIO_BASE) + GPPUD_OFFSET);

/* Initialisation fonction */
static int rbpi_pullupd_init(void)
{
	class_register(&gpio_pud_class);
	return 0;
}

/* Exit fonction */
static void rbpi_pullupd_exit(void)
{
	class_unregister(&gpio_pud_class);
	class_destroy(&gpio_pud_class);

	return;
}

ssize_t gpio_sysfs_pull_up(struct class *class, struct class_attribute *attr,
			   const char *buf, size_t count)
{
	unsigned int gpio;

	if (sscanf(buf, "%d\n", &gpio) != 1)
		return -EINVAL;

	if (!gpio_is_valid(gpio))
		return -EPERM;

	gpio_pull_up(gpio);
	pr_info("Pulling up gpio : %u\n", gpio);
	return count;
}

ssize_t gpio_sysfs_pull_down(struct class *kobj, struct class_attribute *attr,
			   const char *buf, size_t count)
{
	unsigned int gpio;

	if (sscanf(buf, "%d\n", &gpio) != 1)
		return -EINVAL;

	if (!gpio_is_valid(gpio))
		return -EPERM;

	gpio_pull_down(gpio);
	pr_info("Pulling down gpio : %u\n", gpio);
	return count;
}
ssize_t gpio_sysfs_reset(struct class *kobj, struct class_attribute *attr,
			   const char *buf, size_t count)
{
	unsigned int gpio;

	if (sscanf(buf, "%d\n", &gpio) != 1)
		return -EINVAL;

	if (!gpio_is_valid(gpio))
		return -EPERM;
	gpio_reset(gpio);
	pr_info("Reset gpio %d\n", gpio);

	return count;
}

static ssize_t gpio_rbpi_pullupd(struct class *obj, struct class_attribute *attr,
			  char *buff)
{
	return -EPERM;
}

int gpio_pull_up(int gpio)
{
	if (!gpio_is_valid(gpio))
		return -EINVAL;
	gpio_reset(gpio);
	__set_gpud(PULL_UP);
	__setgpudclk(gpio, ENABLE_CLOCK);
	return 0;
}

int gpio_pull_down(int gpio)
{
	if (!gpio_is_valid(gpio))
		return -EINVAL;
	gpio_reset(gpio);
	__set_gpud(PULL_DOWN);
	__setgpudclk(gpio, ENABLE_CLOCK);
	return 0;
}
int gpio_reset(int gpio)
{
	if (!gpio_is_valid(gpio))
		return -EINVAL;
	__set_gpud(PULL_NONE);
	__setgpudclk(gpio, DISABLE_CLOCK);
	return 0;
}

void __set_gpud(uint32_t value)
{
	registers->gppud = value;
	wmb();
	udelay(CYCLE_TO_WAIT);
}
void __setgpudclk(int gpio, uint32_t value)
{
	uint32_t bit;
	uint32_t clk_register_idx;

	bit = gpio % BITS_PER_REGISTER;
	clk_register_idx = gpio / BITS_PER_REGISTER;

	registers->gppudclk[clk_register_idx] =
		(registers->gppudclk[clk_register_idx] & ~(1 << bit))
		| (value << bit);
	wmb();
}

module_init(rbpi_pullupd_init);
module_exit(rbpi_pullupd_exit);
