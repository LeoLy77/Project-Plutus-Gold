/** ***************************************************************
* @file csse4011-s4433912/apps/p2/src/main.c
* @author Leo Ly - 44339124
* @date 25/03/2021
* @brief prac 2 main, AHU driver
* REFERENCE:  
***************************************************************
* EXTERNAL FUNCTIONS 
*************************************************************** 
*************************************************************** */
/* Includes ------------------------------------------------------------------*/

#include <zephyr.h>
#include <device.h>
#include <drivers/uart.h>
#include <usb/usb_device.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <string.h>
#include <errno.h>
#include <sys/byteorder.h>
#include <drivers/spi.h>


#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "s4433912_os_led.h"
#include "s4433912_os_bt_share.h"
#include "os_scu_bt.h"
// #include "awr_spi.h"
/* Private define ------------------------------------------------------------*/
#define STACKSIZE 1024
#define LED_STACKSIZE 300
#define TASK_PRIORITY 14
#define SPI_TASK_PRIORITY 7
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct k_thread task_bt_blink;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
typedef struct Point_t {
	float x; //int16_t
	float y;
} Point;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_PRV_VAL))
};

static void bt_ready(void) {
	int err;

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

float Xarr[] = {0.020827, -0.846722, 0.00105, 0.932754, 0.948919, 0.989678, 1.072909, 
		-1.088779, 0.117878, 1.031307, -1.089547, -1.034791, 1.186756, -1.038733, 0.997182, -0.107075, 
		-1.12528, -1.104855, 1.006652, 1.149408, 0.83698, 1.176405, -0.08708, 1.014404, -1.050965, 
		1.040234, 0.035637, 1.226975, 0.744701, 0.012691, -0.031155, -0.116515, 1.095009, 0.90927, 
		0.148825, -0.040318, 1.097874, 0.936568, 1.086444, 0.046566, -0.876971, -0.984505, -1.16139, 
		1.044386, -0.995424, 0.918685, -1.170627, 1.076104, 1.11394, 1.017743};

float Yarr[] = {0.097664, -0.853064, 0.178587, -1.035955, -1.118063, 1.04106, -0.987102,
		-1.19808, -0.017992, 0.91459, -0.96131, -0.984365, 0.902272, -1.03023, -0.957167, 0.105445,
		-0.922251, -1.142002, -0.969753, 0.979484, -0.953722, 1.040016, -0.057885, 1.145427, 
		-1.043807, -1.068481, 0.070657, 0.854563, 1.065362, 0.040199, 0.005617, 0.090083, 0.984864, 
		-0.994805, 0.189589, 0.122245, 1.224089, -1.036274, 0.925783, -0.153624, -0.879762, -0.962184,
		-1.021274, 1.033367, -1.018718, -1.172628, -0.804922, 1.012168, -1.123483, -1.040178};


void notify(void) {
	static uint8_t arr_size = 5U;

	Point pnt_arr[arr_size];
	for (int i = 0; i < arr_size; i++) {

		pnt_arr[i].x = Xarr[i];
		pnt_arr[i].y = Yarr[i];
	}

	uint8_t data_size = sizeof(pnt_arr);
	uint8_t data[data_size];
    memcpy(data, &pnt_arr, sizeof(pnt_arr));
	printk("[NO STRUCT] %d\n", arr_size);
	send_notification(data, data_size);
	
	arr_size++;
	if (arr_size > 15U) {
		arr_size = 5U;
	}
}


/**
  * @brief zephyr task that tests the log-to-shell functionality
  * @retval 0
  */
void Task_BtLedBlink(void *dummy1, void *dummy2, void *dummy3) {

	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	while(1) {
		s4433912_cli_led_toggle(BLUE_RGB);
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_STACK_DEFINE(btBlink_stack_area, STACKSIZE);


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


void main(void) {

	//Init rgb led gpio
	s4433912_cli_led_init();
	s4433912_cli_led_set(RED_RGB, false);
	s4433912_cli_led_set(GREEN_RGB, false);
	s4433912_cli_led_set(BLUE_RGB, false);

#if defined(CONFIG_USB_UART_CONSOLE)
	const struct device* dev;
	//Link CDC with shell
	dev = device_get_binding(CONFIG_UART_SHELL_ON_DEV_NAME);
	if (dev == NULL || usb_enable(NULL)) {

		return;
	}
	// uint32_t dtr = 0;
	// while (!dtr) {

	// 	uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
	// 	k_sleep(K_MSEC(100));
	// }
#endif
	printk("Starting Bluetooth... ... ..\n");

	/* Initialize the Bluetooth Subsystem */
	int err = bt_enable(NULL);
	if (err) {
		
		s4433912_cli_led_set(RED_RGB, true);
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	k_thread_create(&task_bt_blink, btBlink_stack_area,
		STACKSIZE, Task_BtLedBlink, NULL, NULL, NULL, TASK_PRIORITY, 0, K_NO_WAIT);

	bt_ready();

	bt_conn_cb_register(&scu_conn_callbacks);
	bt_conn_auth_cb_register(&scu_auth_cb_display);

	uint8_t rx_buf[8];
	uint8_t num_points;
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
			k_msleep(20);
		} while (rx_buf[0] != 0xA0);

		memcpy(&num_points, &rx_buf[1], sizeof(num_points));
		printk("Number of Detected Objects: %d\r\n", num_points);

		Point points[num_points];

		for (uint8_t i = 0; i < num_points; i++)
		{
			memset(rx_buf, 0, sizeof(rx_buf));
			spi_receive(rx_buf, 8);

			test = rx_buf[0] << 24 | rx_buf[1] << 16 | rx_buf[2] << 8 | rx_buf[3];
			float x = *((float*)&test);
			if (x > 50) {
				continue;
			}
			// points[i].x = x;
			memcpy(&points[i].x , &x, sizeof(float));
			test = rx_buf[4] << 24 | rx_buf[5] << 16 | rx_buf[6] << 8 | rx_buf[7];
			float y = *((float*)&test);
			if (y > 50) {
				continue;
			}
			// points[i].y = y;
			memcpy(&points[i].y , &y, sizeof(float));
		}	


		uint8_t data_size = sizeof(points);
		uint8_t data[data_size];
		memcpy(data, &points, sizeof(points));
		send_notification(data, data_size);
		
		printk("data_size %d num_points %d\n", data_size, num_points);
		// for (uint8_t i = 0; i < num_points; i++)
		// {
		// 	printk("P %d (%.6f, %.6f)\n",i,  points[i].x, points[i].y);
		// }

	}

	// while (1) {
	// 	k_msleep(500);

	// 	notify();
	// }

}

// K_THREAD_DEFINE(LED_Blink, LED_STACKSIZE, LED_Blink_Task, NULL,NULL,NULL, TASK_PRIORITY, 0, 0);
// K_THREAD_DEFINE(SPI_Receive, STACKSIZE, SPI_Receive_Task, NULL,NULL,NULL, SPI_TASK_PRIORITY, 0, 0);
