import classes
import serial
import threading
import json
import numpy as np
import pandas as pd
import time
import glob
import pylab


def find_serial_port(man=""):
    if len(man) != 0:
        return man
    ports = glob.glob("/dev/tty.usb*")
    print(ports)
    if len(ports) > 1:
        print("More than 1 port found")
        for port in ports:
            if "1434" in port:
                return port

    return ports[0]

# d = {key : [] for key in range(1, NUM_NODES + 1)} #empty dictionary that contains id of mobile nodes
d = {1 : [], 2 : [], 4 : [], 5 : [], 7321 : []}
file_path = "knn(2,1).csv"
def save_data(json_out, lim=150):
    """
    for ML training
    """
    # global d
    id_list = [int(d) for d in str(json_out["static_ids"])]
    rssi_list = [[pkg["rssi"]] for pkg in json_out['data']]

    for i, key in enumerate(id_list):
        d[key].append(rssi_list[i][0])    

    sts = all([len(d[key]) >= lim for key in d]) 
    if sts is True: #check if all nodes have enough data
        #skim all columns to the same size
        for key in d:
            d[key] = d[key][:lim]
        df = pd.DataFrame(data=d)
        df.to_csv(file_path, index=False)
        print("FILE WRITTEN, CLOSING SERIAL COMM...")
        return False

    return True


if __name__ == "__main__":

    serial_conn = serial.Serial(port=find_serial_port(man=""), baudrate = 115200) #SensorTag

    def read_loop(skip=2): #skip every {skip} values, 1 is no skip
        json_ready = False
        recv_cnt = 0
        read_data = True
        while read_data:
            try:

                byte_data = serial_conn.readline()
                recv_cnt += 1
                if recv_cnt % skip != 0:
                    continue
                if recv_cnt == skip:
                    recv_cnt = 0

                data = byte_data.decode('utf-8')
                tmp_data = data.strip()

                data = ''.join(tmp_data)
                if not json_ready:
                    if "[JS_GUD]" in data:
                        json_ready = True
                        
                else:
                    tmp_data = data.split('{', 1)[-1]
                    tmp_data = '{' + tmp_data
                    data = ''.join(tmp_data)

                    json_out = json.loads(data)
                    # read_data = save_data(json_out)
                    json_ready = False

            except Exception as e:
                # print("Exception:", e)
                pass

        # close serial port
        print("close serial port")
        serial_conn.close()

    for i in range(3):
        print("COUNT DOWN ", 3 - i)
        time.sleep(0.3)
    print("STARTING...")
    read_loop()
