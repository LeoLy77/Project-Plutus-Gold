/** ***************************************************************
* @file myoslib/src/s4433912_hal_hci.h
* @author Leo Ly - 44339124
* @date 25/03/2021
* @brief hal hci for SCU and AHU communication
* REFERENCE:  
***************************************************************
* EXTERNAL FUNCTIONS 
*************************************************************** 

*************************************************************** */

#ifndef AHU_BT_H
#define AHU_BT_H

#define DBG_PRINT   0
#define JSON_SCALE_FT   10000
#include <zephyr.h>

#include <string.h>
#include <zephyr/types.h>
#include <sys/util.h>
#include <stddef.h>

#include <errno.h>
#include <init.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <sys/byteorder.h>
#include <data/json.h>

#include "s4433912_os_bt_share.h"

#define MAX_POINTS_LEN 20

typedef struct Point_t {
	float x; //int16_t
	float y;
} Point;

struct bt_point_jsdata {
    uint16_t x;
    uint16_t y;
};

struct bt_frame_jsdata {
    struct bt_point_jsdata data[MAX_POINTS_LEN];
    uint8_t data_len;
};

struct bt_queue_t {
	void* fifo_reserved; /* 1st word reserved for use by fifo */
	struct bt_frame_jsdata frame;
};

extern struct bt_conn_cb ahu_conn_callbacks;
void ahu_start_scan(void);
void Task_Sendjson(void *dummy1, void *dummy2, void *dummy3);

#endif