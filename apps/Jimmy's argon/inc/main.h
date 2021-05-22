#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr.h>
#include <devicetree.h>
#include <device.h>
#include <logging/log.h>
#include <version.h>

#include <drivers/gpio.h>
#include <drivers/spi.h>

#define STACKSIZE 2048
#define TASK_PRIORITY 14

static const struct spi_config spi_cfg_master = {
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .frequency = 500000U,
};

#endif

