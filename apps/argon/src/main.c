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

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "s4433912_os_led.h"
#include "s4433912_os_bt_share.h"
#include "os_scu_bt.h"
#include "awr_spi.h"
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

float Xarr[] = {-1.204752, 1.007982, 0.996618, 0.693641, 1.117449, 0.923881, 1.053264, 
		-0.981406, 1.091325, -1.106534, 1.037568, 1.017285, -1.150335, -1.061158, 0.891124, 
		-0.852365, -1.041749, 0.902422, 1.103732, 0.804376, -1.107456, 0.987614, 1.211686, 
		-0.816066, 1.272371, 1.087491, -1.125826, 1.048281, 1.136728, -1.061297, 1.021291, 
		-1.046479, 1.179289, 0.962614, 0.895504, 1.114011, 1.224107, -1.193668, 0.919305, 1.005491};

float Yarr[] = {-0.765907, -0.963703, -0.9486, 1.078434, 1.268907, -1.043529, 1.040041, 
		-0.95462, 1.014601, -1.237696, 0.897509, 1.174513, -0.906701, -1.052569, -0.993767, 
		-0.855714, -0.981238, -1.207154, 0.91094, -0.944466, -0.953572, 1.049272, 1.048019, 
		-0.823677, 0.825476, -0.984522, -1.170402, -1.082177, -1.148179, -1.141676, -1.048214, 
		-1.036276, 0.975381, -0.99326, -1.069462, 0.981837, 0.882727, -1.025529, -1.043146, 0.977538};


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
	if (arr_size > 40U) {
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

	/* Implement notification. At the moment there is no suitable way
	 * of starting delayed work so we do it here
	 */

	while (1) {
		k_sleep(K_SECONDS(1));

		notify();
	}

}

K_THREAD_DEFINE(LED_Blink, LED_STACKSIZE, LED_Blink_Task, NULL,NULL,NULL, TASK_PRIORITY, 0, 0);
// K_THREAD_DEFINE(SPI_Receive, STACKSIZE, SPI_Receive_Task, NULL,NULL,NULL, SPI_TASK_PRIORITY, 0, 0);
