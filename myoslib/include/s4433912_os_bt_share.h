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

#ifndef OS_BT_SHARE_H
#define OS_BT_SHARE_H

#define MAX_BT_SIZE         20
#define PREAMBLE_SIZE       5
#define DT_LEN_HEADER_SIZE  1
#define MAX_BT_DATA_SIZE    MAX_BT_SIZE - DT_LEN_HEADER_SIZE

#define PREAMBLE_START              0x8F0CEE55
#define PREAMBLE_END                0xCCD07F73

#define BT_UUID_PRV_VAL 0xBBA9
#define BT_UUID_PRV BT_UUID_DECLARE_16(BT_UUID_PRV_VAL)

#endif