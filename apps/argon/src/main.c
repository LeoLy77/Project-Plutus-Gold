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

#include "s4433912_os_bt_share.h"
#include "os_scu_bt.h"

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

void notify(void) {
	static uint8_t heartrate = 90U;

	/* Heartrate measurements simulation */
	heartrate += 2;
	if (heartrate == 160U) {
		heartrate = 90U;
	}

	uint8_t data[DATA_SIZE];
	memset(data, (heartrate - 20), sizeof(uint8_t) * DATA_SIZE);
	data[DATA_SIZE - 1] = heartrate;
	send_notification(data, DATA_SIZE);
}


void main(void) {

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
	printk("Starting Bluetooth... ... ..\n");

	/* Initialize the Bluetooth Subsystem */
	int err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	bt_ready();

	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);

	/* Implement notification. At the moment there is no suitable way
	 * of starting delayed work so we do it here
	 */
	while (1) {
		k_sleep(K_SECONDS(1));

		notify();
	}

}