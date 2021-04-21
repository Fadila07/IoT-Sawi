import paho.mqtt.client as mqtt
import requests


MQTT_ADDRESS = 'ec2-52-90-53-121.compute-1.amazonaws.com'
MQTT_USER = 'frtris'
MQTT_PASSWORD = '123456'
MQTT_TOPIC = 'iot/+/+'
temperature = 0
humidity = 0
moisture = 0
tempT = ''
humT = ''
moisT = ''


def on_connect(client, userdata, flags, rc):
    # """The callback for when the client receives a CONNACK response from the server."""
    # print('Connected with result code' +str(rc))
    if rc == 0:
        client.connected_flag = True  # set flag
        print("connected OK")
        client.subscribe(MQTT_TOPIC)
    else:
        print("Bad connection Returned code=", rc)

def on_message(client, userdata, msg):
    global temperature
    global humidity
    global moisture
    global tempT
    global humT
    global moisT
    """The callback for when a PUBLISH message is received from the server."""
    # print(msg.topic +'' +str(msg.payload))
    if (temperature == 0 or humidity == 0 or moisture == 0):
        if (msg.topic == 'iot/sawi/temperature'):
            temperature = msg.payload.decode("utf-8")
            # temperature = {msg.topic: msg.payload}
            print('temperature = ', temperature)
            tempT = msg.topic
            # r = requests.get('http://192.168.43.67:8080/CobaIoT/sendData.php', ploads)
            # print(r.url)
        elif (msg.topic == 'iot/sawi/humidity'):
            humT = msg.topic
            humidity = msg.payload.decode("utf-8")
            # humidity = {msg.topic: msg.payload}
            print('humidity = ', humidity)
            # r = requests.get('http://192.168.43.67:8080/CobaIoT/sendData.php', ploads)
            # print(r.url)
        elif (msg.topic == 'iot/sawi/moisture'):
            moisT = msg.topic
            moisture = msg.payload.decode("utf-8")
            # moisture = {msg.topic: msg.payload}
            print('moisture = ', moisture)
            # ploads = {msg.topic: msg.payload}
            # r = requests.get('http://192.168.43.67:8080/CobaIoT/sendData.php', ploads)
            # print(r.url)
        # print(temperature, humidity, moisture)
    if(temperature != 0 and humidity != 0 and moisture != 0):

        data = [(tempT, temperature), (humT, humidity), (moisT, moisture)]
        print(temperature, humidity, moisture)
        r = requests.post('http://192.168.43.17:8080/CobaIoT/sendData.php', data)
        print(r.url)
        temperature = 0
        humidity = 0
        moisture = 0
        tempT = ''
        humT = ''
        moisT = ''

def main():
    mqtt_client = mqtt.Client();
    # mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()

if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()