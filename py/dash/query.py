import serial
import time
import json
import threading
import sys

from classes import Point, Cluster
import pandas as pd
import numpy as np
import glob
import matplotlib.pyplot as plt
import re

from datetime import datetime
import time

from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

token = "GXlVuTZaVw2WOnuIo4lMI9-GtbFAjzWCfq3oVjsMrqn6QRzxapVDmsROj3soI_1ESmC5cr41KP0kYxrBy0vzbQ=="
org = "lachlandrury99@gmail.com"
bucket = "csse4011_project"

client = InfluxDBClient(url="https://eu-central-1-1.aws.cloud2.influxdata.com", token=token)

def process_data(data):

    Xs = [i['x'] for i in data]
    Ys = [i['y'] for i in data]
    cluster_in = np.column_stack((Xs, Ys))
    cluster = Cluster(cluster_in, eps=0.35, min_samples=3)
    cluster.plot(fig=plt)
    plt.xlim(-10, 10)
    plt.ylim(-0.9, 18)
    plt.pause(0.00000001)
    plt.clf()

def query_data():

    while(1):
        query = f'from(bucket: "csse4011_project") |> range(start: -2s)'
        tables = client.query_api().query_data_frame(query, org=org)

        records = tables.to_dict('records')

        print(f'{len(records)} New Records')

        points=[]

        for entry in records:
            point = {"x": entry["x"], "y": entry["y"], "velocity": entry["_value"]}
            points.append(point)
            print(point)

        process_data(points)

        time.sleep(1)

query_data()