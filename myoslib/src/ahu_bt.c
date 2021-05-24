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

#include <ahu_bt.h>
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct bt_conn *default_conn;

static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
K_FIFO_DEFINE(bt_queue_fifo);

struct json_obj_descr js_descr[] = {
	JSON_OBJ_DESCR_PRIM(struct bt_point_jsdata, x, JSON_TOK_NUMBER),
	JSON_OBJ_DESCR_PRIM(struct bt_point_jsdata, y, JSON_TOK_NUMBER),
};

struct json_obj_descr js_descr_array[] = {
	JSON_OBJ_DESCR_OBJ_ARRAY(struct bt_frame_jsdata, data, MAX_POINTS_LEN,
							data_len, js_descr, ARRAY_SIZE(js_descr))};
/**
  * @brief  pack bluetooth uuid data to json format to send over serial communication
  * @param 	val - bluetooth data struct
  */
void bt_data2json(struct bt_frame_jsdata* jsdata) {

	printk("Queue recv len %d\n", jsdata->data_len);

	// printk("Queue recv len %d\n", jsdata->data_len);
	// for (int i = 0; i < jsdata->data_len; i++) {

	// 	printk("P %d, (x, y) = (%d, %d)\n", i, jsdata->data[i].x, jsdata->data[i].y);
	// }

	// ssize_t js_len = json_calc_encoded_len(&js_descr_array, sizeof(js_descr_array), jsdata);
	// printk("[JS LEN] %d\n", js_len);
    
    // char le_buffer[500];

    // int ret = json_obj_encode_buf(&js_descr_array, sizeof(js_descr_array),
	// 			  &jsdata, le_buffer, sizeof(le_buffer));

    // if (ret != 0) {

    //     printk("[JS_BAD] %d\n", ret);
    // } else {

    //     printk("[JS_GUD]\n"); //Start piping json to serial
    //     printk("%s\n", le_buffer);
    // }
}

void Task_Sendjson(void *dummy1, void *dummy2, void *dummy3) {

	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	struct bt_queue_t* rx_data;
	printk("Task_Sendjson started\n");
	while(1) {

		rx_data = k_fifo_get(&bt_queue_fifo, K_FOREVER);
		// struct bt_frame_jsdata frame = rx_data->frame;
		printk("Queue recv len %d\n", rx_data->data_len);
		// bt_data2json(&rx_data->frame);
		// k_free(rx_data);
	}
}

void bt_data2queue(Point* raw_pnt_data, uint8_t num_packets) {

	uint8_t struct_len = num_packets;
	struct bt_point_jsdata pnt_data[struct_len];
	for (uint8_t i = 0; i < struct_len; i++) {
		// pnt_data[i].x = (int16_t) (JSON_SCALE_FT * raw_pnt_data[i].x);
		// pnt_data[i].y = (int16_t) (JSON_SCALE_FT * raw_pnt_data[i].y);
		printk("%d (x, y) = (%.6f, %.6f)\n", i, raw_pnt_data[i].x, raw_pnt_data[i].y);
	}
	// struct bt_queue_t* tx_data = k_malloc(sizeof(struct bt_queue_t));

	// memcpy(&tx_data->frame.data, pnt_data, sizeof(pnt_data)); 
	// tx_data->data_len = struct_len;

	// struct bt_queue_t tx_data;
	// tx_data.data_len = struct_len;
	// k_fifo_put(&bt_queue_fifo, &tx_data);

	// printk("Queued %d\n", tx_data.data_len);
	// k_free(tx_data);
}

static void process_bt_data(const uint8_t *raw_data, uint16_t length) {

	static uint8_t data_buffer[1224];

	static uint8_t running = 0;
	static uint8_t pkg_len = 0;
	static uint8_t total_data_len = 0;
	if (length == PREAMBLE_SIZE) {

    	int pre;
    	memcpy(&pre, raw_data, sizeof(uint8_t)*4);//Preample minus data length

		if (pre == PREAMBLE_START && running == 0){

			running = 1;
			pkg_len = raw_data[PREAMBLE_SIZE - 1];
#if DBG_PRINT
			printk("[HANDSHAKE MADE]\n");
#endif
			return;

		//BT transmission finished, ready for the next step
		} else if (pre == PREAMBLE_END && running == 1) {
			
			uint8_t struct_len = ((total_data_len*sizeof(uint8_t)) / sizeof(Point));
#if DBG_PRINT
			printk("[RECV] %d STRUCTs\n", struct_len);
#endif
			Point recv;
			printk("[JS_GUD]\n");

			printk("{\"frame\":[");
			for (int i = 0; i < struct_len; i++) {
    			memcpy(&recv, &data_buffer[sizeof(Point)*i], sizeof(Point));
				//{i:, x:, y:}
				printk("{\"i\":%d,\"x\":%.3f,\"y\":%.3f}", i, recv.x, recv.y);
				if (i != struct_len - 1) {
					printk(",");
				}
			}
			printk("]}\n");

			pkg_len = 0;
			total_data_len = 0;
			running = 0;
#if DBG_PRINT
			printk("[CONN ENDED]\n");
#endif
			return;
		} 
		
		if (running == 0) {
#if DBG_PRINT
			printk("[BAD HANDSHAKE]");
			for (int i = 0; i < length; i++) {
				printk("%X ", raw_data[i]);
			}
			printk("\n");
#endif
			running = 0;
			pkg_len = 0;
			total_data_len = 0;
			return;
		}
	}
	//Never reaches here without clearing the preample
	uint8_t part_num = raw_data[0];
	if (part_num > pkg_len) {
#if DBG_PRINT
		printk("[%d CONFLICT PKG ID %X]\n", pkg_len, part_num);
#endif
		running = 0;
		pkg_len = 0;
		total_data_len = 0;
		return;
	}

	for (uint8_t i = DT_LEN_HEADER_SIZE, j = total_data_len; i < length; i++, j++) {

		memcpy(&data_buffer[j], &raw_data[i], sizeof(uint8_t));
	}
	total_data_len += (length - DT_LEN_HEADER_SIZE);
}

static uint8_t notify_func(struct bt_conn *conn,
			   struct bt_gatt_subscribe_params *params,
			   const void *raw_data, uint16_t length) {
	if (!raw_data) {
		printk("[UNSUBSCRIBED]\n");
		params->value_handle = 0U;
		return BT_GATT_ITER_STOP;
	}
	// uint8_t* data = (uint8_t*) raw_data;
	// printk("[NOTIFICATION size %d]\n", length);

	process_bt_data((uint8_t*) raw_data, length);
	return BT_GATT_ITER_CONTINUE;
}

static uint8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params) {
	int err;

	if (!attr) {
		printk("Discover complete\n");
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	printk("[ATTRIBUTE] handle %u\n", attr->handle);

	if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_PRV)) {

		memcpy(&uuid, BT_UUID_HRS_MEASUREMENT, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.start_handle = attr->handle + 1;
		discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			printk("Discover failed (err %d)\n", err);
		}
	} else if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_HRS_MEASUREMENT)) {

		memcpy(&uuid, BT_UUID_GATT_CCC, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.start_handle = attr->handle + 2;
		discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
		subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

		err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			printk("Discover failed (err %d)\n", err);
		}
	} else {

		subscribe_params.notify = notify_func;
		subscribe_params.value = BT_GATT_CCC_NOTIFY;
		subscribe_params.ccc_handle = attr->handle;

		err = bt_gatt_subscribe(conn, &subscribe_params);
		if (err && err != -EALREADY) {
			printk("Subscribe failed (err %d)\n", err);
		} else {
			printk("[SUBSCRIBED]\n");
		}

		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_STOP;
}

static bool eir_found(struct bt_data *data, void *user_data) {
	bt_addr_le_t *addr = user_data;
	int i;

	printk("[AD]: %u data_len %u\n", data->type, data->data_len);

	switch (data->type) {
	case BT_DATA_UUID16_SOME:
	case BT_DATA_UUID16_ALL:
		if (data->data_len % sizeof(uint16_t) != 0U) {
			printk("AD malformed\n");
			return true;
		}

		for (i = 0; i < data->data_len; i += sizeof(uint16_t)) {
			struct bt_le_conn_param *param;
			struct bt_uuid *uuid;
			uint16_t u16;
			int err;

			memcpy(&u16, &data->data[i], sizeof(u16));
			uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));
			if (bt_uuid_cmp(uuid, BT_UUID_PRV)) {
				continue;
			}

			err = bt_le_scan_stop();
			if (err) {
				printk("Stop LE scan failed (err %d)\n", err);
				continue;
			}

			param = BT_LE_CONN_PARAM_DEFAULT;
			err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
						param, &default_conn);
			if (err) {
				printk("Create conn failed (err %d)\n", err);
				ahu_start_scan();
			}

			return false;
		}
	}

	return true;
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	char dev[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(addr, dev, sizeof(dev));
	printk("[DEVICE]: %s, AD evt type %u, AD data len %u, RSSI %i\n",
	       dev, type, ad->len, rssi);

	/* We're only interested in connectable events */
	if (type == BT_GAP_ADV_TYPE_ADV_IND ||
	    type == BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
		bt_data_parse(ad, eir_found, (void *)addr);
	}
}

void ahu_start_scan(void) {
	int err;

	/* Use active scanning and disable duplicate filtering to handle any
	 * devices that might update their advertising data at runtime. */
	struct bt_le_scan_param scan_param = {
		.type       = BT_LE_SCAN_TYPE_ACTIVE,
		.options    = BT_LE_SCAN_OPT_NONE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
	};

	err = bt_le_scan_start(&scan_param, device_found);
	if (err) {
		printk("Scanning failed to start (err %d)\n", err);
		return;
	}

	printk("Scanning successfully started\n");
}

static void connected(struct bt_conn *conn, uint8_t conn_err) {
    
	char addr[BT_ADDR_LE_STR_LEN];
	int err;

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (conn_err) {
		printk("Failed to connect to %s (%u)\n", addr, conn_err);

		bt_conn_unref(default_conn);
		default_conn = NULL;

		ahu_start_scan();
		return;
	}

	printk("Connected: %s\n", addr);

	if (conn == default_conn) {
		memcpy(&uuid, BT_UUID_PRV, sizeof(uuid));
		discover_params.uuid = &uuid.uuid;
		discover_params.func = discover_func;
		discover_params.start_handle = 0x0001;
		discover_params.end_handle = 0xffff;
		discover_params.type = BT_GATT_DISCOVER_PRIMARY;

		err = bt_gatt_discover(default_conn, &discover_params);
		if (err) {
			printk("Discover failed(err %d)\n", err);
			return;
		}
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {

	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Disconnected: %s (reason 0x%02x)\n", addr, reason);

	if (default_conn != conn) {
		return;
	}

	bt_conn_unref(default_conn);
	default_conn = NULL;

	ahu_start_scan();
}

// static struct bt_conn_cb conn_callbacks = {
struct bt_conn_cb ahu_conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};
