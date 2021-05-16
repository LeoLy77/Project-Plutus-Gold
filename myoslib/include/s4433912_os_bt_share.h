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

#ifndef S4433912_OS_BT_SHARE_H
#define S4433912_OS_BT_SHARE_H

#include <bluetooth/uuid.h>
#include <bluetooth/bluetooth.h>

#define BT_UUID_VAL 0xBBA9
#define BT_UUID_PRV BT_UUID_DECLARE_16(BT_UUID_VAL)

#endif