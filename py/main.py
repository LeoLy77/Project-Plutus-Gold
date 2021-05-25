from classes import Point, Cluster
import serial
import json
import pandas as pd
import numpy as np
import time
import glob
import matplotlib.pyplot as plt
import re


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

def json_process(json_out):
    data = json_out["frame"]
    Xs = [i['x'] for i in data]
    Ys = [i['y'] for i in data]
    cluster_in = np.column_stack((Xs, Ys))
    cluster = Cluster(cluster_in)
    cluster.plot(fig=plt)
    # plt.scatter(Xs, Ys)
    plt.pause(0.000001)
    plt.clf()
    # grp = []
    # for pnt in data:
    #     p = Point(x=pnt['x'], y=pnt['y'])
    #     grp.append(p)

if __name__ == "__main__":

    # 7-bit C1 ANSI sequences
    ansi_escape = re.compile(r'''
        \x1B  # ESC
        (?:   # 7-bit C1 Fe (except CSI)
            [@-Z\\-_]
        |     # or [ for CSI, followed by a control sequence
            \[
            [0-?]*  # Parameter bytes
            [ -/]*  # Intermediate bytes
            [@-~]   # Final byte
        )
    ''', re.VERBOSE)

    serial_conn = serial.Serial(port=find_serial_port(man="/dev/tty.usbmodem145401"), baudrate = 115200) #SensorTag
    plt.figure()
    plt.xlim(-5, 5)
    plt.ylim(-5, 5)
    def read_loop(skip=1): #skip every {skip} values, 1 is no skip
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
                # print(data)
                if not json_ready:
                    if "[JS_GUD]" in data:
                        json_ready = True
                        
                else:

                    data = ansi_escape.sub('', data)

                    json_out = json.loads(data)
                    json_process(json_out)
                    # read_data = save_data(json_out)
                    json_ready = False

            except Exception as e:
                json_ready = False
                print("Exception:", e)
                pass

        # close serial port
        print("close serial port")
        serial_conn.close()

    for i in range(3):
        print("COUNT DOWN ", 3 - i)
        time.sleep(0.1)
    print("STARTING...")
    read_loop()
