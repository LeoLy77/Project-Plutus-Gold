#ifndef AWR_SPI_H
#define AWR_SPI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr.h>
#include <devicetree.h>
#include <device.h>
#include <logging/log.h>
#include <version.h>

#include <zephyr/types.h>
#include <sys/printk.h>
#include <sys/util.h>

#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <usb/usb_device.h>
#include <drivers/uart.h>

#include "s4433912_os_bt_share.h"
#include "os_scu_bt.h"

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)

typedef struct Point_t {
	float x; //int16_t
	float y;
} Point;

void LED_Blink_Task(void);
void SPI_Receive_Task(void);

#endif