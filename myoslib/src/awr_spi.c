#include <awr_spi.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)

const struct device *spi_dev;
static const struct spi_config spi_cfg_master = {
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPHA,
	.frequency = 10000000U,
};

/* Function Definitions */
uint8_t spi_receive(uint8_t* data_buf, uint32_t data_len) 
{
	uint8_t err;
	/* Create spi structs */
    struct spi_buf buf = { .buf = data_buf, .len = data_len };
    struct spi_buf_set buf_set = { .buffers = &buf, .count = 1 };

	/* Receive via spi */
	err = spi_read(spi_dev, &spi_cfg_master, &buf_set);
	// err = spi_transceive(spi_dev, &spi_cfg_slave, NULL, &buf_set);

	return err;
}

void spi_transmit(uint8_t *data_buf, uint8_t data_len) {
    uint8_t err = 1;

    /* Create spi structs */
    struct spi_buf buf = { .buf = data_buf, .len = data_len };
    struct spi_buf_set buf_set = { .buffers = &buf, .count = 1 };

	err = spi_write(spi_dev, &spi_cfg_master, &buf_set);
    return;
}

/* Tasks */
void LED_Blink_Task(void) 
{
	const struct device *gpio_dev = device_get_binding(LED0);
    gpio_pin_configure(gpio_dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	bool is_on = true;

	while(1)
	{
		gpio_pin_set(gpio_dev, PIN, (int)is_on);
		is_on = !is_on;
		k_msleep(1000);
	}
}

void SPI_Receive_Task(void) 
{
	uint8_t rx_buf[8];
	uint8_t** rx_buffer;
	uint8_t num_points;
	Point points[64];
	uint32_t test;

	spi_dev = device_get_binding("SPI_3");
	if (spi_dev == NULL) {
		printk("SPI could not be initialised");
    } else {
		printk("SPI has successfully initialised");
    }

	while(1) {
		do {
			memset(rx_buf, 0, sizeof(rx_buf));
			spi_receive(rx_buf, 8);
			k_msleep(200);
		} while (rx_buf[0] != 0xA0);

		num_points = rx_buf[1];
		printk("Number of Detected Objects: %d\r\n", num_points);

		for (uint8_t i = 0; i < num_points; i++)
		{
			memset(rx_buf, 0, sizeof(rx_buf));
			spi_receive(rx_buf, 8);

			test = rx_buf[0] << 24 | rx_buf[1] << 16 | rx_buf[2] << 8 | rx_buf[3];
			points[i].x = *((float*)&test);
			test = rx_buf[4] << 24 | rx_buf[5] << 16 | rx_buf[6] << 8 | rx_buf[7];
			points[i].y = *((float*)&test);
		}	

		for (uint8_t i = 0; i < num_points; i++)
		{
			printk("(%.6f, %.6f)\n", points[i].x, points[i].y);
		}

		memset(points, 0, sizeof(Point) * num_points);

	}
}