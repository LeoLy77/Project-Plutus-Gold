/** ***************************************************************
* @file 
* @author Leo Ly - 44339124
* @date 
* @brief 
* REFERENCE:  
***************************************************************
* EXTERNAL FUNCTIONS 
*************************************************************** 
*************************************************************** */
/* Includes ------------------------------------------------------------------*/

#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <usb/usb_device.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <sys/byteorder.h>

#include "ahu_bt.h"
#include "s4433912_os_bt_share.h"
/* Private define ------------------------------------------------------------*/
#define STACKSIZE 1024
#define TASK_PRIORITY 5
#define LED_STACKSIZE 200
#define LED_TASK_PRIORITY 14

#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void Task_Blinky(void *dummy1, void *dummy2, void *dummy3) {

	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	const struct device *led_dev = device_get_binding(LED0);
	if (led_dev == NULL) {
		return;
	}

	int ret = gpio_pin_configure(led_dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	gpio_pin_set(led_dev, PIN, 1);

	while (1) {
		gpio_pin_toggle(led_dev, PIN);
		k_msleep(900);
	}
}

void main(void) {
	int err;
#if defined(CONFIG_USB_UART_CONSOLE)
	const struct device* dev;
	uint32_t dtr = 0;
	//Link CDC with shell
	dev = device_get_binding(CONFIG_UART_SHELL_ON_DEV_NAME);
	if (dev == NULL || usb_enable(NULL)) {

		return;
	}

	while (!dtr) {

		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		k_sleep(K_MSEC(100));
	}
#endif
	printk("Starting Bluetooth... ... ...\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(NULL);
	if (err) {

		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized successfully\n");

	bt_conn_cb_register(&ahu_conn_callbacks);

	ahu_start_scan();
}

// K_THREAD_DEFINE(SendJsonPID, STACKSIZE, Task_Sendjson, NULL, NULL, NULL, TASK_PRIORITY, 0, 0);
K_THREAD_DEFINE(blinkyPID, LED_STACKSIZE, Task_Blinky, NULL, NULL, NULL, LED_TASK_PRIORITY, 0, 0);
