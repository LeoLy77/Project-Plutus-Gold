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

#include <drivers/uart.h>
#include <usb/usb_device.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <sys/byteorder.h>

#include "ahu_bt.h"
#include "s4433912_os_led.h"
#include "s4433912_os_bt_share.h"
/* Private define ------------------------------------------------------------*/
#define PRIORITY 6
#define STACKSIZE 500
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct k_thread task_bt_blink;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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

void main(void) {
	//Init rgb led gpio
	s4433912_cli_led_init();
	s4433912_cli_led_set(RED_RGB, false);
	s4433912_cli_led_set(GREEN_RGB, false);
	s4433912_cli_led_set(BLUE_RGB, false);

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

		s4433912_cli_led_set(RED_RGB, true);
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	k_thread_create(&task_bt_blink, btBlink_stack_area,
		STACKSIZE, Task_BtLedBlink, NULL, NULL, NULL, PRIORITY, 0, K_NO_WAIT);

	printk("Bluetooth initialized successfully\n");

	bt_conn_cb_register(&ahu_conn_callbacks);

	ahu_start_scan();
}