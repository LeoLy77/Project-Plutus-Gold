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

#ifndef S4433912_OS_BT_H
#define S4433912_OS_BT_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <sys/byteorder.h>
#include <string.h>
#include <zephyr/types.h>
#include <data/json.h>

#include "s4433912_os_bt_share.h"

int pv_bt_notify(uint16_t heartrate);

#endif