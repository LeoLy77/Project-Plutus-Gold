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

typedef struct Point_t {
	float x;
	float y;
} Point;

void LED_Blink_Task(void);
void SPI_Receive_Task(void);

#endif