import os
import django
import json
import paho.mqtt.client as mqtt
import time
import sys
import argparse
# ARGS
parser = argparse.ArgumentParser()
parser.add_argument(
    "-t", "--topic",
    type=str,
    default="huanga9/ranger_led",
    help="MQTT topic to subscribe to"
)
args = parser.parse_args()

MQTT_TOPIC = args.topic

# --- Django Setup ---
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "dashboard.settings")
django.setup()

# --- Configuration ---
MQTT_BROKER = "129.161.190.151"
MQTT_PORT = 1883

from web.models import DistanceReading, WeightReading, RFIDReading

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker")
        client.subscribe(MQTT_TOPIC)
        print(f"Subscribed to: {MQTT_TOPIC}")
    else:
        print(f"Failed to connect, return code {rc}")

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        print("Data Parsed:", data)
        if "distance" in data:
            DistanceReading.objects.create(
                value=float(data["distance"])
            )
        if "left_weight" in data and "right_weight" in data:
            WeightReading.objects.create(
                left=float(data["left_weight"]),
                right=float(data["right_weight"])
            )
        if "rfid" in data:
            RFIDReading.objects.create(
                mode=data["rfid"]
            )
    except Exception as e:
        print(f"Error: {e}")

def main():
    try:
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
    except AttributeError:
        client = mqtt.Client() 

    client.on_connect = on_connect
    client.on_message = on_message

    print(f" Connecting to {MQTT_BROKER}...")
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 180)
    except Exception as e:
        print(f"Could not connect to broker: {e}")
        sys.exit(1)

    # loop_start() runs the network loop in a background thread
    client.loop_start()

    print("listener is running.")

    while True:
        # Keep the main thread alive
        time.sleep(1)

if __name__ == "__main__":
    main()