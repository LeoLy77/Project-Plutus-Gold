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

#ifndef PRO_OS_SCU_BT_H
#define PRO_OS_SCU_BT_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <sys/byteorder.h>
#include <string.h>
#include <zephyr/types.h>

#include "s4433912_os_bt_share.h"

extern struct bt_conn_cb conn_callbacks;
extern struct bt_conn_auth_cb auth_cb_display;

int send_notification(uint8_t* data, uint8_t length);

#endif