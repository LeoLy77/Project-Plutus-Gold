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

#include <errno.h>
#include <zephyr.h>
#include <init.h>
#include <data/json.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <sys/byteorder.h>

#include <string.h>
#include <zephyr/types.h>
#include <sys/util.h>
#include <stddef.h>

// #include "awr_spi.h"
#include "s4433912_os_bt_share.h"

typedef struct Point_t {
	float x; //int16_t
	float y;
} Point;

struct bt_point_jsdata {
    int16_t x;
    int16_t y;
};

struct bt_frame_jsdata {
    struct bt_point_jsdata* data;
    uint8_t data_len;
};

extern struct bt_conn_cb ahu_conn_callbacks;
void ahu_start_scan(void);

#endif