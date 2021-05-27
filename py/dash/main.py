from classes import Point, Cluster
import serial
import json
import pandas as pd
import numpy as np
import time
import glob
import matplotlib.pyplot as plt
import re

from datetime import datetime

from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

# You can generate a Token from the "Tokens Tab" in the UI
token = "GXlVuTZaVw2WOnuIo4lMI9-GtbFAjzWCfq3oVjsMrqn6QRzxapVDmsROj3soI_1ESmC5cr41KP0kYxrBy0vzbQ=="
org = "lachlandrury99@gmail.com"
bucket = "csse4011_project"

client = InfluxDBClient(url="https://eu-central-1-1.aws.cloud2.influxdata.com", token=token)

write_api = client.write_api(write_options=SYNCHRONOUS)

def write_data(data):
    global write_api, bucket, org

    data = data["frame"]

    points = []

    for entry in data:
        _point = Point("data") \
            .tag("x", entry["x"]) \
            .tag("y", entry["y"]) \
            .field("velocity", 0) \
            .time(datetime.utcnow(), WritePrecision.S)

        points.append(_point)

    print(f'Sending {len(data)} Points')
    write_api.write(bucket=bucket, record=points, org=org)

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

def json_process(json_out):
    data = json_out["frame"]
    Xs = [i['x'] for i in data]
    Ys = [i['y'] for i in data]
    cluster_in = np.column_stack((Xs, Ys))
    cluster = Cluster(cluster_in, eps=0.35, min_samples=3)
    cluster.plot(fig=plt)
    plt.xlim(-10, 10)
    plt.ylim(-0.9, 18)
    plt.pause(0.00000001)
    plt.clf()

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

    serial_conn = serial.Serial(port=find_serial_port(man=""), baudrate = 115200) #SensorTag
    plt.figure()
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
                    write_data(json_out)
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
