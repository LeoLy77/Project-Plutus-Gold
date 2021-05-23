/** ***************************************************************
* @file myoslib/src/s4433912_os_bt.c
* @author Leo Ly - 44339124
* @date 20/04/2021
* @brief bluetooth os support for communication between nodes
* REFERENCE:  
***************************************************************
* EXTERNAL FUNCTIONS 
*************************************************************** 
*************************************************************** */

/* Includes ------------------------------------------------------------------*/
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr.h>
#include <init.h>
#include <sys/printk.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "os_scu_bt.h"
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static uint8_t blsc;

static void hrmc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	printk("Notifications %s \n", notif_enabled ? "enabled" : "disabled");
}

static ssize_t read_blsc(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset) {
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &blsc,
				 sizeof(blsc));
}

/* Heart Rate Service Declaration */
BT_GATT_SERVICE_DEFINE(bt_svc,

	BT_GATT_PRIMARY_SERVICE(BT_UUID_PRV),
	BT_GATT_CHARACTERISTIC(BT_UUID_HRS_MEASUREMENT, BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_NONE, NULL, NULL, NULL),
	BT_GATT_CCC(hrmc_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_HRS_BODY_SENSOR, BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, read_blsc, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_HRS_CONTROL_POINT, BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_NONE, NULL, NULL, NULL),
);

static int init(const struct device *dev) {

	ARG_UNUSED(dev);

	blsc = 0x01;

	return 0;
}

int send_notification(uint8_t* raw_data, uint8_t length) {
	
	int rc, err = 0;
    uint8_t mac = MAX_BT_DATA_SIZE;
	uint8_t last_el_size = (length % mac);
	uint8_t bt_arr_len = 0;
	
	bt_arr_len = length/(MAX_BT_DATA_SIZE+ 0.0);
	if (last_el_size > 0) {
		bt_arr_len ++;
	}
	printk("last_el_size = %d, bt_arr_len = %d\n", last_el_size, bt_arr_len);

	// //Send preamble and Data length
	uint8_t preamble_data[PREAMBLE_SIZE];
	for (int i = 0; i < 4; i++) {

		preamble_data[i] = ((PREAMBLE_START >> 8*(i)) & 0xFF);
	}
	preamble_data[PREAMBLE_SIZE - 1] = bt_arr_len; //Number of bluetooth packages 

	rc = bt_gatt_notify(NULL, &bt_svc.attrs[1], &preamble_data, sizeof(uint8_t)*PREAMBLE_SIZE);
	err = (rc == -ENOTCONN ? 0 : rc);
	if (err == 0) {

		printk("[SEND] PREAMBLE START\n");
	} else {

		printk("[SEND BAD] PREAMBLE START %d\n", bt_arr_len);
		return err;
	}
	//Now send the data
	uint8_t itr_cnt = 0;
	for (int i = 0; i < bt_arr_len; i++) {

		uint8_t arr_len = MAX_BT_DATA_SIZE;
		if (i == (bt_arr_len - 1)) {
			if (bt_arr_len == 1) {

				arr_len = length;
			} else {
				arr_len = last_el_size;
			}
		}

		uint8_t data[arr_len + DT_LEN_HEADER_SIZE];
		data[0] = (uint8_t) (i + 1);

		for (int k = 1, j = itr_cnt; k < arr_len + DT_LEN_HEADER_SIZE; k++, j++) {
			memcpy(&data[k], &raw_data[j], sizeof(uint8_t));
		}

        itr_cnt += arr_len;
        
		rc = bt_gatt_notify(NULL, &bt_svc.attrs[1], &data, sizeof(uint8_t)*(arr_len + DT_LEN_HEADER_SIZE));
		err = (rc == -ENOTCONN ? 0 : rc);

		if (err == 0) {

			printk("[SEND] ");
			// for (int i = 0; i < length; i++) {
			// 	printk("%X ", data[i]);
			// }
			// printk("\n");
		} else {

			printk("[SEND BAD] NOTI\n");
			return err;
		}
	}

	//finish
	for (int i = 0; i < 4; i++) {

		preamble_data[i] = ((PREAMBLE_END >> 8*(i)) & 0xFF);
	}

	rc = bt_gatt_notify(NULL, &bt_svc.attrs[1], &preamble_data, sizeof(uint8_t)*PREAMBLE_SIZE);
	err = (rc == -ENOTCONN ? 0 : rc);
	if (err == 0) {

		printk("[SEND] PREAMBLE END\n");
	} else {

		printk("[SEND BAD] PREAMBLE END\n");
		return err;
	}

	return err;

}


SYS_INIT(init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

struct bt_conn_cb scu_conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

static void auth_cancel(struct bt_conn *conn) {
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

struct bt_conn_auth_cb scu_auth_cb_display = {
	.cancel = auth_cancel,
};

