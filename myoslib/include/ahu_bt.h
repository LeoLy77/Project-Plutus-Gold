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

#include <errno.h>
#include <zephyr.h>
#include <init.h>

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

#include "s4433912_os_bt_share.h"

// struct bt_parse_data {
//     uint8_t valid;
//     uint8_t static_uuid;
//     uint16_t ulsd;
// };


struct bt_static_jsdata {
    int16_t x; //-32767 -> 32767
    int16_t y;
    uint16_t vel;
};

struct bt_node_jsdata {
    struct bt_static_jsdata* data;
    uint8_t data_len;
};

extern struct bt_conn_cb ahu_conn_callbacks;
void ahu_start_scan(void);

#endif